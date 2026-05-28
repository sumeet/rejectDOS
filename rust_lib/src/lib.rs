#![no_std]
#![no_main]

use core::panic::PanicInfo;

#[no_mangle]
pub extern "C" fn eval(mut input_ptr: *const u8, input_len: u16) -> u16 {
    let mut res = 0;
    let mut temp = 0;
    let mut op = b'+';
    let end_ptr = unsafe { input_ptr.offset(input_len as isize) };
    
    while input_ptr < end_ptr {
        let c = unsafe { *input_ptr };
        if c >= b'0' && c <= b'9' {
            temp = temp * 10 + (c - b'0') as u16;
        } else if c == b'+' || c == b'-' {
            if op == b'+' { res += temp; }
            else if op == b'-' { res -= temp; }
            temp = 0;
            op = c;
        }
        input_ptr = unsafe { input_ptr.offset(1) };
    }
    if op == b'+' { res += temp; }
    else if op == b'-' { res -= temp; }
    
    res
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
