# CTS: C Type System

## Introduction

Hi there! I'm supernovayuli, the creator of the **C Type System (CTS)**, a modular, class-like runtime type system written entirely in C. This document serves as a detailed explanation and documentation of how CTS works, and how it's meant to be used. If you've stumbled into this codebase, i severly apologize for what you are about to read. It's a bit unconventional, but it gives us powerful polymorphism and type introspection in C.

---

## The CTS Philosophy

CTS implements a runtime type system in C. Think of it as object-oriented programming, but without the C++ overhead (or safety). We create *types*, then *instances* of those types, and provide a flexible way to handle inheritance, virtual function calls, serialization, deserialization, and type-safe introspection.

Everything is structured around two central concepts:

* **TypeDefinition**: Describes a type (like a class). It includes handlers for functions (like methods).
* **TypeInstance**: Represents an instance of a type (like an object). It stores a `TypeID` and a pointer to raw data.

Instances can be null (null instance wrapper), typed-null (valid instance wrapper, missing data), corrupted (missing type, has data), or valid.

---

## Core Structures

### `TypeID`, `SubID`, `FunID`

These are typedefs and macros that help us refer to types, functions, and subfields in a unified way:

* `TypeID` = 16-bit identifier for each type.
* `FunID` = 32-bit identifier combining type and local function ID.
* `SubID` = 32-bit identifier combining type and local subfield ID.

We use macros like `t_formfun`, `t_gettypefromfun`, `t_formsub`, etc., to create and dissect these IDs.

### `TypeDefinition`

```c
struct TypeDefinition {
    TypeID id;
    char* name;
    bool (*has_func)(FunID id);
    void (*call_func)(FunCall* call);
};
```

Each type registers a `TypeDefinition`. It provides a function table and a function dispatcher.

### `TypeInstance`

```c
struct TypeInstance {
    TypeID id;
    void* data;
    size_t references;
};
```

This represents a single instance of a type. We store the type ID and a pointer to its data struct.

---

## Function Calls

Functions (aka methods) are dispatched by `FunID`.

```c
struct FunCall {
    FunID target;
    char code;
    void* argstruct;
};
```

We define function handlers as:

```c
void f_TYPENAME_imp_FUNCTIONNAME(F_FUNCTIONNAME_PRM* prm, char* code)
```

These are stored in a function table and dispatched by `call_func`.

Macros help automate boilerplate:

* `DEF_FUN` declares a function.
* `IMP_FUN` implements it.
* `BEGIN_TYPE_FUNCTIONS`, `TYPE_FUNCTION`, `END_TYPE_FUNCTIONS` build function tables.
* `REGISTER_TYPE` registers the type.

---

## Inheritance and Subfields

CTS supports basic inheritance through subfield access:

* Each type must handle `FID_DEF_GETSUB` and `FID_DEF_HASSUB`.
* These use `SubID`, which combines type + local subfield ID.

This lets us implement polymorphism:
If a `Circle` contains a `Shape` as a `TypeInstance`, and implements `getsub(ShapeID, 0)`, then we can treat a `Circle` as a `Shape`.

---

## Instance/Reference Management (r\_ functions)

`InstanceReference` is a lightweight reference wrapper for a `TypeInstance*`, with utilities for:

* Creating: `r_create(TypeID)`
* Cloning: `r_createfrom(InstanceReference)`
* Destruction: `r_destroy`, `r_forget`
* Wrapping: `r_createfor(TypeInstance*)`
* Subfields: `r_def_getsub`, `r_getself`

---

## Serialization & Deserialization

Each type implements `FID_DEF_SERIALIZE` and `FID_DEF_DESERIALIZE`. When serializing:

1. CTS writes the `TypeID` to a `MemoryBlock`.
2. It calls the type's serialize handler to write the data.

Deserialization reads back the `TypeID`, creates an instance, and reads the data.

This allows binary save/load and clone operations.

---

## Summary of Core Concepts

| Concept              | Description                               |
| -------------------- | ----------------------------------------- |
| TypeDefinition       | a class declaration                       |
| TypeInstance         | an object instance                        |
| Function Call        | Runtime-dispatched function               |
| SubID                | Inheritance access (self or nested field) |
| InstanceReference    | Ref-counted instance wrapper              |
| r\_def\_*, i\_def\_* | Wrapper utilities for easier use          |

---

## Final Notes

* CTS is **not thread-safe**.
* Always serialize nested `TypeInstance`s properly.
* Treat corrupted/null/typed-null values carefully.
* Inheritance is manual, via `getsub` and nested `TypeInstance`s.
* CTS is flexible but requires discipline!

I hope this helps clarify how the system is structured and meant to be used. Happy hacking!
