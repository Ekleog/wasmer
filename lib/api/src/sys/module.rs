use crate::sys::store::Store;
use crate::sys::InstantiationError;
use std::fmt;
use std::io;
use std::path::Path;
use std::sync::Arc;
use thiserror::Error;
use wasmer_compiler::CompileError;
#[cfg(feature = "wat")]
use wasmer_compiler::WasmError;
use wasmer_engine::RuntimeError;
use wasmer_engine_universal::CodeMemory;
use wasmer_engine_universal::UniversalArtifact;
use wasmer_engine_universal::UniversalEngine;
use wasmer_types::InstanceConfig;
use wasmer_vm::{InstanceHandle, Instantiatable, Resolver};

#[derive(Error, Debug)]
pub enum IoCompileError {
    /// An IO error
    #[error(transparent)]
    Io(#[from] io::Error),
    /// A compilation error
    #[error(transparent)]
    Compile(#[from] CompileError),
}

/// A WebAssembly Module contains stateless WebAssembly
/// code that has already been compiled and can be instantiated
/// multiple times.
///
/// ## Cloning a module
///
/// Cloning a module is cheap: it does a shallow copy of the compiled
/// contents rather than a deep copy.
#[derive(Clone)]
pub struct Module {
    store: Store,
    artifact: Arc<wasmer_engine_universal::UniversalArtifact>,
}

impl Module {
    /// Creates a new WebAssembly Module given the configuration
    /// in the store.
    ///
    /// If the provided bytes are not WebAssembly-like (start with `b"\0asm"`),
    /// and the "wat" feature is enabled for this crate, this function will try to
    /// to convert the bytes assuming they correspond to the WebAssembly text
    /// format.
    ///
    /// ## Security
    ///
    /// Before the code is compiled, it will be validated using the store
    /// features.
    ///
    /// ## Errors
    ///
    /// Creating a WebAssembly module from bytecode can result in a
    /// [`CompileError`] since this operation requires to transorm the Wasm
    /// bytecode into code the machine can easily execute.
    ///
    /// ## Example
    ///
    /// Reading from a WAT file.
    ///
    /// ```
    /// use wasmer::*;
    /// # fn main() -> anyhow::Result<()> {
    /// # let store = Store::default();
    /// let wat = "(module)";
    /// let module = Module::new(&store, wat)?;
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Reading from bytes:
    ///
    /// ```
    /// use wasmer::*;
    /// # fn main() -> anyhow::Result<()> {
    /// # let store = Store::default();
    /// // The following is the same as:
    /// // (module
    /// //   (type $t0 (func (param i32) (result i32)))
    /// //   (func $add_one (export "add_one") (type $t0) (param $p0 i32) (result i32)
    /// //     get_local $p0
    /// //     i32.const 1
    /// //     i32.add)
    /// // )
    /// let bytes: Vec<u8> = vec![
    ///     0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x60,
    ///     0x01, 0x7f, 0x01, 0x7f, 0x03, 0x02, 0x01, 0x00, 0x07, 0x0b, 0x01, 0x07,
    ///     0x61, 0x64, 0x64, 0x5f, 0x6f, 0x6e, 0x65, 0x00, 0x00, 0x0a, 0x09, 0x01,
    ///     0x07, 0x00, 0x20, 0x00, 0x41, 0x01, 0x6a, 0x0b, 0x00, 0x1a, 0x04, 0x6e,
    ///     0x61, 0x6d, 0x65, 0x01, 0x0a, 0x01, 0x00, 0x07, 0x61, 0x64, 0x64, 0x5f,
    ///     0x6f, 0x6e, 0x65, 0x02, 0x07, 0x01, 0x00, 0x01, 0x00, 0x02, 0x70, 0x30,
    /// ];
    /// let module = Module::new(&store, bytes)?;
    /// # Ok(())
    /// # }
    /// ```
    #[allow(unreachable_code)]
    pub fn new(
        store: &Store,
        bytes: impl AsRef<[u8]>,
        code_memory: &mut CodeMemory,
    ) -> Result<Self, CompileError> {
        #[cfg(feature = "wat")]
        let bytes = wat::parse_bytes(bytes.as_ref()).map_err(|e| {
            CompileError::Wasm(WasmError::Generic(format!(
                "Error when converting wat: {}",
                e
            )))
        })?;

        Self::from_binary(store, bytes.as_ref(), code_memory)
    }

    /// Creates a new WebAssembly module from a file path.
    pub fn from_file(
        store: &Store,
        file: impl AsRef<Path>,
        code_memory: &mut CodeMemory,
    ) -> Result<Self, IoCompileError> {
        let file_ref = file.as_ref();
        let wasm_bytes = std::fs::read(file_ref)?;
        let module = Self::new(store, &wasm_bytes, code_memory)?;
        // Set the module name to the absolute path of the filename.
        // This is useful for debugging the stack traces.
        Ok(module)
    }

    /// Creates a new WebAssembly module from a binary.
    ///
    /// Opposed to [`Module::new`], this function is not compatible with
    /// the WebAssembly text format (if the "wat" feature is enabled for
    /// this crate).
    pub fn from_binary(
        store: &Store,
        binary: &[u8],
        code_memory: &mut CodeMemory,
    ) -> Result<Self, CompileError> {
        Self::validate(store, binary)?;
        unsafe { Self::from_binary_unchecked(store, binary, code_memory) }
    }

    /// Creates a new WebAssembly module skipping any kind of validation.
    ///
    /// # Safety
    ///
    /// This can speed up compilation time a bit, but it should be only used
    /// in environments where the WebAssembly modules are trusted and validated
    /// beforehand.
    pub unsafe fn from_binary_unchecked(
        store: &Store,
        binary: &[u8],
        code_memory: &mut CodeMemory,
    ) -> Result<Self, CompileError> {
        let module = Self::compile(store, binary, code_memory)?;
        Ok(module)
    }

    /// Validates a new WebAssembly Module given the configuration
    /// in the Store.
    ///
    /// This validation is normally pretty fast and checks the enabled
    /// WebAssembly features in the Store Engine to assure deterministic
    /// validation of the Module.
    pub fn validate(store: &Store, binary: &[u8]) -> Result<(), CompileError> {
        store.engine().validate(binary)
    }

    fn compile(
        store: &Store,
        binary: &[u8],
        code_memory: &mut CodeMemory,
    ) -> Result<Self, CompileError> {
        match store.engine().downcast_ref::<UniversalEngine>() {
            Some(universal_engine) => {
                let executable = universal_engine.compile_universal(binary, store.tunables())?;
                let artifact =
                    Arc::new(universal_engine.load_universal_executable(code_memory, &executable)?);
                Ok(Self::from_universal_artifact(store, artifact))
            }
            None => panic!("unknown engine type"),
        }
    }

    /// Make a Module from Artifact...
    pub fn from_universal_artifact(
        store: &Store,
        artifact: Arc<wasmer_engine_universal::UniversalArtifact>,
    ) -> Self {
        Self {
            store: store.clone(),
            artifact,
        }
    }

    pub(crate) fn instantiate(
        &self,
        resolver: &dyn Resolver,
        config: InstanceConfig,
    ) -> Result<InstanceHandle, InstantiationError> {
        unsafe {
            let instance_handle = Arc::clone(&self.artifact).instantiate(
                self.store.tunables(),
                resolver,
                Box::new((self.store.clone(), Arc::clone(&self.artifact))),
                config,
            )?;

            // After the instance handle is created, we need to initialize
            // the data, call the start function and so. However, if any
            // of this steps traps, we still need to keep the instance alive
            // as some of the Instance elements may have placed in other
            // instance tables.
            instance_handle
                .finish_instantiation()
                .map_err(|t| InstantiationError::Start(RuntimeError::from_trap(t)))?;

            Ok(instance_handle)
        }
    }

    /// Returns the [`Store`] where the `Instance` belongs.
    pub fn store(&self) -> &Store {
        &self.store
    }
}

impl fmt::Debug for Module {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Module").finish()
    }
}
