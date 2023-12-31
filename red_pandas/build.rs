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
use std::env;


fn main() {

    // https://doc.rust-lang.org/cargo/reference/environment-variables.html

    let out_dir = env::var("OUT_DIR").unwrap();
    let package_name = env::var("CARGO_PKG_NAME").unwrap();
    let python_home = env::var("PYTHON_HOME").unwrap();
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    cmake::build(".");
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
