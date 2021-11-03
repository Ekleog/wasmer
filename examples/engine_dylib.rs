//! Defining an engine in Wasmer is one of the fundamental steps.
//!
//! This example illustrates how to use the `wasmer_engine_dylib`,
//! aka the Dylib engine. An engine applies roughly 2 steps:
//!
//!   1. It compiles the Wasm module bytes to executable code, through
//!      the intervention of a compiler,
//!   2. It stores the executable code somewhere.
//!
//! In the particular context of the Dylib engine, the executable code
//! is stored in a shared object (`.dylib`, `.so` or `.dll` file).
//!
//! You can run the example directly by executing in Wasmer root:
//!
//! ```shell
//! cargo run --example engine-dylib --release --features "cranelift"
//! ```
//!
//! Ready?

use std::fmt::Write;
use wasmer::{imports, wat2wasm, Instance, Module, Store, Value};
use wasmer_compiler_cranelift::Cranelift;
use wasmer_engine_dylib::Dylib;

pub fn many_functions_contract(function_count: u32) -> Vec<u8> {
    let mut functions = String::new();
    for i in 0..function_count {
        writeln!(
            &mut functions,
            "(func
            i32.const {}
            drop
            return)",
            i
        )
        .unwrap();
    }

    let code = format!(
        r#"(module
        (export "main" (func 0))
        {})"#,
        functions
    );
    wat2wasm(code.as_bytes()).unwrap().to_vec()
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_span_tree::span_tree().aggregate(true).enable();

    // Let's declare the Wasm module with the text representation.
    let wasm_bytes = many_functions_contract(150_000);

    // Define a compiler configuration.
    //
    // In this situation, the compiler is
    // `wasmer_compiler_cranelift`. The compiler is responsible to
    // compile the Wasm module into executable code.
    let compiler_config = Cranelift::default();

    println!("Creating Dylib engine...");
    // Define the engine that will drive everything.
    //
    // In this case, the engine is `wasmer_engine_dylib` which means
    // that a shared object is going to be generated.
    let engine = Dylib::new(compiler_config).engine();

    // Create a store, that holds the engine.
    let store = Store::new(&engine);

    println!("Compiling module...");
    // Here we go.
    //
    // Let's compile the Wasm module. It is at this step that the Wasm
    // text is transformed into Wasm bytes (if necessary), and then
    // compiled to executable code by the compiler, which is then
    // stored into a shared object by the engine.
    let module = {
        let _span = tracing::debug_span!(target: "vm", "Module::new (compile)").entered();
        Module::new(&store, wasm_bytes)?
    };

    // Congrats, the Wasm module is compiled! Now let's execute it for
    // the sake of having a complete example.

    // Create an import object. Since our Wasm module didn't declare
    // any imports, it's an empty object.
    let import_object = imports! {};

    println!("Instantiating module...");
    // And here we go again. Let's instantiate the Wasm module.
    let instance = {
        let _span = tracing::debug_span!(target: "vm", "Instance::new").entered();
        Instance::new(&module, &import_object)?
    };

    println!("Instantiating module... the second time");
    let instance = {
        // This one matches NEAR's execution model of initialization
        let _span = tracing::debug_span!(target: "vm", "Instance::new").entered();
        Instance::new(&module, &import_object)?
    };

    println!("Calling `sum` function...");
    // The Wasm module exports a function called `sum`.
    let sum = instance.exports.get_function("sum")?;
    let results = sum.call(&[Value::I32(1), Value::I32(2)])?;

    println!("Results: {:?}", results);
    assert_eq!(results.to_vec(), vec![Value::I32(3)]);

    Ok(())
}

#[test]
#[cfg(not(any(target_arch = "aarch64", target_env = "musl")))]
fn test_engine_dylib() -> Result<(), Box<dyn std::error::Error>> {
    main()
}
