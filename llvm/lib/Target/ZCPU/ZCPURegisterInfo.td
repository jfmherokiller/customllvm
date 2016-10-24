//===-- ZCPURegisterInfo.td - ZCPU Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the ZCPU register file
//===----------------------------------------------------------------------===//


// We have banks of 16 registers each.
class ZCPURegBase< string n> : Register<n> {
  //let HWEncoding = Enc;
  let Namespace = "ZCPU";
}

// ZCPU CPU Registers
class ZCPUReg< string n> : ZCPURegBase< n>;

//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
let Namespace = "ZCPU" in {
  //@ General Purpose Registers

  def IP   : ZCPUReg<"IP">;
}

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def Regs : RegisterClass<"ZCPU", [i48] , 48 , (add IP)>;