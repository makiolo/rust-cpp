// use cc;
//
// fn main() {
//     cc::Build::new()
//         .cpp(true)
//         .file("main.cpp")
//         .compile("hello_cpp");
// }

use cmake;
use std::path::Path;
use std::path::PathBuf;
use std::env;



fn main() {

    // https://doc.rust-lang.org/cargo/reference/environment-variables.html

    // #![allow(non_upper_case_globals)]
    // #![allow(non_camel_case_types)]
    // #![allow(non_snake_case)]
    // include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

    let out_dir = env::var("OUT_DIR").unwrap();
    let package_name = env::var("CARGO_PKG_NAME").unwrap();
    let python_home = env::var("PYTHON_HOME").unwrap();
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();



    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    /*
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("wrapper.h")
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
    */


    let dst = cmake::Config::new(".")
        .generator("Ninja")
        .profile("Release")
        // .build_target("red_pandas_static")
        .define("CMAKE_C_COMPILER", "D:/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang-cl.exe")
        .define("CMAKE_CXX_COMPILER", "D:/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang-cl.exe")
        .define("CMAKE_C_FLAGS", "-m64")
        .define("CMAKE_CXX_FLAGS", "-m64")
        .define("CMAKE_MAKE_PROGRAM", "D:/CLion/bin/ninja/win/x64/ninja.exe")
        .define("CMAKE_TOOLCHAIN_FILE", "C:/Users/makiolo/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
        .build();

    println!("cargo:rustc-link-search=all={}\\..\\bin", out_dir);
    println!("cargo:rustc-link-search=all={}\\Library\\bin", python_home);
    println!("cargo:rustc-link-search=all={}\\Library\\lib", python_home);
    println!("cargo:rustc-link-search=all={}\\libs", python_home);
    println!("cargo:rustc-link-search={}", Path::new(&dir).join("bin").display());
    if cfg!(windows) {
        let profile = env::var("PROFILE").unwrap();
        println!("cargo:rustc-link-search=all={}\\build\\{}", out_dir, profile);
    } else if cfg!(unix) {
        println!("cargo:rustc-link-search=all={}/build", out_dir);
    } else {
        println!("Invalid OS: {}", env::consts::OS);
    }

    println!("cargo:rustc-link-lib=static={}", "red_pandas_static");
    println!("cargo:rustc-link-lib=static={}", "mkl_intel_lp64_dll");
    println!("cargo:rustc-link-lib=static={}", "mkl_intel_thread_dll");
    println!("cargo:rustc-link-lib=static={}", "mkl_core_dll");
    println!("cargo:rustc-link-lib=static={}", "ta_lib_a");
    println!("cargo:rustc-link-lib=static={}", "portfolio");
    println!("cargo:rustc-link-lib=static={}", "simulator");
    println!("cargo:rustc-link-lib=static={}", "npv_static");
}
