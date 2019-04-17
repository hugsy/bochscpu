use std::slice;

use crate::PhyAddress;
use crate::syncunsafecell::SyncUnsafeCell;

mod fnv_mem;
pub use fnv_mem::{add_page, del_page, resolve_hva, resolve_hva_checked};


#[ctor]
static FAULT: SyncUnsafeCell<Box<FnMut(PhyAddress)>> = {
    SyncUnsafeCell::new(Box::new(|_| panic!("no missing_page function set")))
};

const fn page_off(a: PhyAddress) -> (PhyAddress, usize) {
    (a & !0xfff, a as usize & 0xfff)
}

pub unsafe fn fault(gpa: PhyAddress) {
    let f = FAULT.0.get();
    (**f)(gpa);
}

#[no_mangle]
extern "C" fn mem_guest_to_host(gpa: PhyAddress, _rw: u32) -> *mut u8 {
    trace!("translating guest phys {:x}...", gpa);

    unsafe { translate(gpa) }
}

#[no_mangle]
extern "C" fn mem_read_phy(gpa: PhyAddress, sz: u32, dst: *mut u8) {
    trace!("mem read {} bytes from phys {:x}...", sz, gpa);

    let sz = sz as usize;

    unsafe {
        let src_ptr = translate(gpa);
        let src = slice::from_raw_parts(src_ptr, sz);
        let dst = slice::from_raw_parts_mut(dst, sz);

        dst.copy_from_slice(src);
        trace!("mem read {:x?}", src);
    }
}

#[no_mangle]
extern "C" fn mem_write_phy(gpa: PhyAddress, sz: u32, src: *const u8) {
    trace!("mem write {} bytes to phys {:x}...", sz, gpa);

    let sz = sz as usize;

    unsafe {
        let dst_ptr = translate(gpa);
        let dst = slice::from_raw_parts_mut(dst_ptr, sz);
        let src = slice::from_raw_parts(src, sz);

        dst.copy_from_slice(src);
        trace!("mem write {:x?}", src);
    }
}

pub unsafe fn translate(gpa: PhyAddress) -> *mut u8{
    if let Some(hva) = resolve_hva_checked(gpa) {
        return hva;
    }

    fault(gpa);

    resolve_hva(gpa)
}

pub unsafe fn missing_page<T: FnMut(PhyAddress) + 'static>(f: T) {
    *(FAULT.0.get()) = Box::new(f);
}
