#![no_core]
#![feature(no_core, lang_items)]

#[lang = "sized"]
pub trait Sized {}

#[lang = "pointee_sized"]
pub trait PointeeSized {}

#[lang = "copy"]
pub trait Copy {}

#[lang = "freeze"]
pub trait Freeze {}

#[no_mangle]
pub extern "C" fn eval(mut input_ptr: *const u8, input_len: u16) -> u16 {
    let mut res = 0;
    let mut temp = 0;
    let mut op = 43; // '+' is 43
    let end_ptr = unsafe { input_ptr.offset(input_len as isize) };
    
    while input_ptr < end_ptr {
        let c = unsafe { *input_ptr };
        if c >= 48 && c <= 57 { // '0' is 48, '9' is 57
            temp = temp * 10 + (c - 48) as u16;
        } else if c == 43 || c == 45 { // '+' is 43, '-' is 45
            if op == 43 { res += temp; }
            else if op == 45 { res -= temp; }
            temp = 0;
            op = c;
        }
        input_ptr = unsafe { input_ptr.offset(1) };
    }
    if op == 43 { res += temp; }
    else if op == 45 { res -= temp; }
    
    res
}
