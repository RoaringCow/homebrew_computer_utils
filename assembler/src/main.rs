use std::io::{BufRead, BufReader};
use std::fs::File;
use std::env;

fn main() {


    // Collect command-line arguments
    let args: Vec<String> = env::args().collect();

    // Check if the correct number of arguments is provided
    if args.len() != 2 {
        eprintln!("Usage: {} <file_path>", args[0]);
        std::process::exit(1);
    }

    // Get the file path from the arguments
    let file_path = &args[1];

    // Open the file
    let file = match File::open(file_path) {
        Ok(file) => file,
        Err(err) => {
            eprintln!("Error opening file {}: {}", file_path, err);
            std::process::exit(1);
        }
    };

    let reader = BufReader::new(file);

    let mut file_lines: Vec<String> = Vec::new();
    // Read the file line by line
    for line in reader.lines() {
        match line {
            Ok(line) => file_lines.push(line),
            Err(err) => {
                eprintln!("Error reading line: {}", err);
                std::process::exit(1);
            }
        }
    }


    // output binary
    let mut binary: Vec<u8> = Vec::new();

    for line in file_lines.iter() {
        let parts: Vec<&str> = line.split_whitespace().collect();

        if parts.len() > 3 {
            panic!("Too many arguments in a line. example: ld A, B (so 3 :) )");
        }


        println!("{}", line);

        // match opcode
        match parts[0] {
            "ld" => {
                let operand_one: String = parts[1].to_string().replace(",", "");
                let operand_two: String = parts[2].to_string().replace(",", "");
                

                let mut destination_str: Option<&str> = None;
                let mut source_str: Option<&str> = None;
                // check if operations with ram ..asd.asdas.dasd
                if operand_one.chars().nth(0) == Some('[') && operand_one.chars().nth(operand_one.len() - 1) == Some(']') {
                    destination_str = Some(&operand_one[1..operand_one.len()-1]);
                }

                if operand_two.chars().nth(0) == Some('[') && operand_two.chars().nth(operand_two.len() - 1) == Some(']') {
                    source_str = Some(&operand_two[1..operand_two.len()-1]);
                } 

                // well this might not be a great way of making it but its 1.30 am 
                if let Some(dest) = destination_str {
                    if source_str != None {
                        panic!("address to address not yet supported");
                    } else {
                        if operand_two.contains("0x") {
                            binary.push(0x18);
                            match u8::from_str_radix(&operand_two.trim_start_matches("0x"), 16){
                                Ok(v) => {
                                    binary.push(v);
                                },
                                Err(_) => panic!("?"),
                        
                            }
                        }else {
                            match operand_two.as_str() {
                                "A" => binary.push(0x14), // ld a8, a
                                "B" => binary.push(0x15),
                                _ => panic!("that source operand is not supported or invalid"),
                            } 
                        }
                    }
                    match u8::from_str_radix(dest.trim_start_matches("0x"), 16){
                        Ok(v) => binary.push(v),
                        Err(_) => panic!("destination address is not a hex number"),
                    }
                } else {
                    if let Some(source) = source_str {
                        match operand_one.as_str() {
                            "A" => binary.push(0x12),
                            "B" => binary.push(0x13),
                            _ => panic!("that source operand is not supported or invalid"),
                        }
                        match u8::from_str_radix(source.trim_start_matches("0x"), 16){
                            Ok(v) => binary.push(v),
                            Err(_) => panic!("source address is not a hex number -> {}", source),
                        }
                    }else {
                        if operand_two.contains("0x"){
                            match operand_one.as_str() {
                                "A" => binary.push(0x16),
                                "B" => binary.push(0x17),
                                _ => panic!("that source operand is not supported or invalid"),
                            }
                            match u8::from_str_radix(&operand_two.trim_start_matches("0x"), 16) {
                                Ok(v) => binary.push(v),
                                Err(_) => panic!("you wrote the hex code wrong {}", operand_two),
                            }

                        }else {
                            match operand_one.as_str() {
                                "A" => binary.push(0x10),
                                "B" => binary.push(0x11),
                                _ => panic!("idk what you're doing but dont do that look at the syntax docs"),
                            }
                        }
                    }
                }
            },
            "add" => {
                // there is only one instruction on this
                binary.push(0x20);
            },
            "sub" => {
                // also only one instruction
                binary.push(0x30);
            },
            _ => panic!("my shitty assembler didnt understand your instruction"),
        }
    }
    for byte in binary.iter() {
        println!("{:x}", byte);
    }
}