// use cc;
//
// fn main() {
//     cc::Build::new()
//         .cpp(true)
//         .file("main.cpp")
//         .compile("hello_cpp");
// }

use cmake;
use std::env;


fn main() {

    // https://doc.rust-lang.org/cargo/reference/environment-variables.html

    let out_dir = env::var("OUT_DIR").unwrap();
    let profile = env::var("PROFILE").unwrap();
    let package_name = env::var("CARGO_PKG_NAME").unwrap();

    cmake::build(".");

    println!("cargo:rustc-link-search=all={}\\build\\{}", out_dir, profile);
    println!("cargo:rustc-link-lib=static={}", package_name);
}

