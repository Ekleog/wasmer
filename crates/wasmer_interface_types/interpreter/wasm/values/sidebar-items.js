initSidebarItems({"enum":[["DeserializeError","Represents an error while deserializing."],["InterfaceValue","A WIT value."],["SerializeError","Represents an error while serializing."]],"fn":[["from_interface_values","Deserialize a set of `InterfaceValue`s to a type `T` that implements the `Deserialize` trait."],["to_interface_value","Serialize a type `T` that implements the `Serialize` trait to an `InterfaceValue`."]],"struct":[["FlattenInterfaceValueIterator","Iterates over a vector of `InterfaceValues` but flatten all the values. So `I32(1), Record([I32(2), I32(3)]), I32(4)` will be iterated like `I32(1), I32(2), I32(3), I32(4)`."]],"trait":[["NativeType","Represents a native type supported by WIT."]]});