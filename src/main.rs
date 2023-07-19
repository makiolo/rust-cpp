use std::fs;
use std::io::prelude::*;
use std::net::TcpListener;
use std::net::TcpStream;
use std::thread;
use std::{
    sync::{mpsc, Arc, Mutex}
};
use std::fmt::Debug;
use serde::{Serialize, Deserialize};
use http_parser::{HttpParser, HttpParserType, CallbackResult, ParseAction, HttpParserCallback};

use hello_cpp::{sum};
use std::collections::HashMap;

/*
#[link(name = "hello_cpp")]
extern "C" {
    fn sum(left: f64, right: f64) -> f64;
}
*/

#[derive(Serialize, Deserialize, Debug)]
struct Point {
    x: i32,
    y: i32,
}

#[no_mangle]
pub extern "C" fn call_from_c() {
    println!("Just called a Rust function from C!");
}



fn main() {

    let point = Point { x: 1, y: 2 };

    // Convert the Point to a JSON string.
    let serialized = serde_json::to_string(&point).unwrap();

    println!("Point serialized: {}", serialized);

    // Convert the JSON string back to a Point.
    let deserialized: Point = serde_json::from_str(&serialized).unwrap();

    // Prints deserialized = Point { x: 1, y: 2 }
    println!("Point deserialized = {:?}", deserialized);

    /////////////////

    println!("listening 127.0.0.1:7878 ...");

    let num_workers = 16;

    let listener = TcpListener::bind("127.0.0.1:7878").unwrap();
    let pool = ThreadPool::new(num_workers);

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        pool.execute(|| {
            handle_connection(stream);
        });
    }

    println!("Shutting down.");
}

struct Callback;

impl HttpParserCallback for Callback {

    fn on_message_begin(&mut self, _parser: &mut HttpParser) -> CallbackResult {
        println!("Message begin");
        Ok(ParseAction::None)
    }

    fn on_body(
        &mut self,
        _parser: &mut HttpParser,
        data: &[u8]) -> CallbackResult
    {
        let body = std::str::from_utf8(data).expect("Invalid utf-8");
        println!("on body: {}", body);
        Ok(ParseAction::None)
    }

    fn on_url(&mut self, _parser: &mut HttpParser, data: &[u8]) -> CallbackResult
    {
        let body = std::str::from_utf8(data).expect("Invalid utf-8");
        println!("on url: {}", body);

        let index = body.find("?").unwrap_or(0);
        let body_trunc = &body[index + 1..];

        let split = body_trunc.split("&");
        let mut mymap = HashMap::new();
        for s in split {
            let chunks : Vec<&str> = s.split("=").collect();
            let mut key: Option<&str> = None;
            let mut value: Option<f64> = None;
            let mut i = 0;
            for c in chunks {
                match i {
                    0 => {key = Option::from(c);},
                    1 => {value = Option::from(c.parse::<f64>().unwrap());},
                    _ => {}
                }
                i += 1;
            }
            if let Some(v) = value
            {
                mymap.insert(key.unwrap(), v);
            }
        }
        unsafe {
            let left = *mymap.entry("left").or_insert(0.0);
            let right = *mymap.entry("right").or_insert(0.0);

            let result_sum = sum(left, right);

            println!("sum = {}", result_sum);
        }

        Ok(ParseAction::None)
    }

    fn on_status(
        &mut self,
        _parser: &mut HttpParser,
        data: &[u8]
    ) -> CallbackResult
    {
        let body = std::str::from_utf8(data).expect("Invalid utf-8");
        println!("on status: {}", body);
        Ok(ParseAction::None)
    }
}

fn handle_connection(mut stream: TcpStream) {

    let mut buffer = [0; 4096];
    stream.read(&mut buffer).unwrap();

    let mut parser = HttpParser::new(HttpParserType::Request);
    let mut cb = Callback;
    parser.execute(&mut cb, &buffer);

    let (status_line, filename) = ("HTTP/1.1 200 OK", "hello.html");

    let contents = fs::read_to_string(filename).unwrap();
    let mut cont_owned = contents.to_owned();

    let r2 = format!("{}", 1234.0);
    cont_owned.push_str(r2.as_str());

    let response = format!(
        "{}\r\nContent-Length: {}\r\n\r\n{}",
        status_line,
        cont_owned.len(),
        cont_owned
    );

    stream.write_all(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}

pub struct ThreadPool {
    workers: Vec<Worker>,
    sender: Option<mpsc::Sender<Job>>,
}

type Job = Box<dyn FnOnce() + Send + 'static>;

impl ThreadPool {
    /// Create a new ThreadPool.
    ///
    /// The size is the number of threads in the pool.
    ///
    /// # Panics
    ///
    /// The `new` function will panic if the size is zero.
    pub fn new(size: usize) -> ThreadPool {
        assert!(size > 0);

        let (sender, receiver) = mpsc::channel();

        let receiver = Arc::new(Mutex::new(receiver));

        let mut workers = Vec::with_capacity(size);

        for id in 0..size {
            workers.push(Worker::new(id, Arc::clone(&receiver)));
        }

        ThreadPool {
            workers,
            sender: Some(sender),
        }
    }

    pub fn execute<F>(&self, f: F)  where F: FnOnce() + Send + 'static,
    {
        let job = Box::new(f);

        self.sender.as_ref().unwrap().send(job).unwrap();
    }
}

impl Drop for ThreadPool {
    fn drop(&mut self) {
        drop(self.sender.take());

        for worker in &mut self.workers {
            println!("Shutting down worker {}", worker.id);

            if let Some(thread) = worker.thread.take() {
                thread.join().unwrap();
            }
        }
    }
}

struct Worker {
    id: usize,
    thread: Option<thread::JoinHandle<()>>,
}

impl Worker {
    fn new(id: usize, receiver: Arc<Mutex<mpsc::Receiver<Job>>>) -> Worker {
        let thread = thread::spawn(move || loop {
            let message = receiver.lock().unwrap().recv();

            match message {
                Ok(job) => {
                    println!("Worker {id} got a job; executing.");

                    job();
                }
                Err(_) => {
                    println!("Worker {id} disconnected; shutting down.");
                    break;
                }
            }
        });

        Worker {
            id,
            thread: Some(thread),
        }
    }
}