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

    let out_dir = env::var("OUT_DIR").unwrap();
    let profile = env::var("PROFILE").unwrap();
    println!("OUT_DIR = {}", out_dir);

    cmake::build(".");

    println!("cargo:rustc-link-search=all={}\\build\\{}", out_dir, profile);
    println!("cargo:rustc-link-lib=static={}", "hello_cpp");
}

