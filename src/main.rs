// use std::fs;
// use std::io::prelude::*;
// use std::net::TcpListener;
// use std::net::TcpStream;
// use std::thread;
// use std::{
//     sync::{mpsc, Arc, Mutex}
// };
use std::fmt::Debug;
use serde::{Serialize, Deserialize};
use test_cpp::sum;
use red_pandas::custommain;


extern crate rocket;


#[derive(Serialize, Deserialize, Debug)]
struct Point {
    x: i32,
    y: i32,
}

#[derive(Serialize, Deserialize, Debug)]
struct Result {
    response: f64,
}

#[rocket::get("/<name>/<left>/<right>")]
fn entry_sum(name: &str, left: f64, right: f64) -> String {

    let mut result : f64 = 0.0;
    match name {
        "sum" => {
            unsafe {
                result = sum(left, right);
            }
        }
        _ => (),
    };
    let struct_result = Result{ response: result };
    serde_json::to_string(&struct_result).unwrap()
}

#[rocket::get("/<x>/<y>")]
fn test(x: i32, y: i32) -> String {

    let point = Point { x, y };

    // Convert the Point to a JSON string.
    let serialized = serde_json::to_string(&point).unwrap();

    println!("Point serialized: {}", serialized);

    // Convert the JSON string back to a Point.
    let deserialized: Point = serde_json::from_str(&serialized).unwrap();

    // Prints deserialized = Point { x: 1, y: 2 }
    println!("Point deserialized = {:?}", deserialized);

    unsafe {
        custommain();
    }

    serialized
}

#[rocket::launch]
fn rocket() -> _ {
    rocket::build()
        .mount("/calculator", rocket::routes![entry_sum])
        .mount("/struct", rocket::routes![test])
}

