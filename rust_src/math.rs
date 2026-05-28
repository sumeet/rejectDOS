#![no_std]
#![no_main]

use core::panic::PanicInfo;

#[no_mangle]
pub extern "C" fn eval(mut input_ptr: *const u8, input_len: u16) -> u16 {
    let mut res: u16 = 0;
    let mut temp: u16 = 0;
    let mut op: u8 = b'+';
    let end_ptr = unsafe { input_ptr.offset(input_len as isize) };
    
    while input_ptr < end_ptr {
        let c = unsafe { *input_ptr };
        
        let mut is_op = false;
        if c == b'+' { is_op = true; }
        if c == b'-' { is_op = true; }
        if c == b'*' { is_op = true; }
        if c == b'/' { is_op = true; }
        if c == b'%' { is_op = true; }
        
        if c >= b'0' && c <= b'9' {
            temp = temp.wrapping_mul(10).wrapping_add((c - b'0') as u16);
        } else if is_op {
            if op == b'+' { res = res.wrapping_add(temp); }
            if op == b'-' { res = res.wrapping_sub(temp); }
            if op == b'*' { res = res.wrapping_mul(temp); }
            if op == b'/' {
                if temp != 0 { res = res.wrapping_div(temp); }
                else { res = 0; }
            }
            if op == b'%' {
                if temp != 0 { res = res.wrapping_rem(temp); }
                else { res = 0; }
            }
            temp = 0;
            op = c;
        }
        input_ptr = unsafe { input_ptr.offset(1) };
    }
    
    if op == b'+' { res = res.wrapping_add(temp); }
    if op == b'-' { res = res.wrapping_sub(temp); }
    if op == b'*' { res = res.wrapping_mul(temp); }
    if op == b'/' {
        if temp != 0 { res = res.wrapping_div(temp); }
        else { res = 0; }
    }
    if op == b'%' {
        if temp != 0 { res = res.wrapping_rem(temp); }
        else { res = 0; }
    }
    
    res
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
