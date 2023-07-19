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
    let package_name = env::var("CARGO_PKG_NAME").unwrap();

    cmake::build(".");

    if cfg!(windows) {
        let profile = env::var("PROFILE").unwrap();
        println!("cargo:rustc-link-search=all={}\\build\\{}", out_dir, profile);
    } else if cfg!(unix) {
        println!("cargo:rustc-link-search=all={}/build", out_dir);
    } else {
        println!("Invalid OS: {}", env::consts::OS);
    }

    println!("cargo:rustc-link-lib=static={}", package_name);
}

