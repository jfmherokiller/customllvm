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
//@ All registers definition
let Namespace = "ZCPU" in {

  def IP   : ZCPUReg<"IP">;
  def EAX  : ZCPUReg<"EAX">;
  def EBX  : ZCPUReg<"EBX">;
  def EDX  : ZCPUReg<"EDX">;
  def ECX  : ZCPUReg<"ECX">;

}

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def ProtectRegs : RegisterClass<"ZCPU", [i48] , 48 , (add IP)>;
def Int48: RegisterClass<"ZCPU", [i48] , 48 , (add EAX,EBX)>;
def Float64: RegisterClass<"ZCPU", [f64] , 64 , (add EAX,EBX)>;
