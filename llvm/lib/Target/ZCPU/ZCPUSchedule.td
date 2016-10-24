//===-- ZCPUSchedule.td - ZCPU Scheduling Definitions ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Functional units across ZCPU chips sets. Based on GCC/ZCPU backend files.
//===----------------------------------------------------------------------===//
def ALU     : FuncUnit;


//===----------------------------------------------------------------------===//
// Instruction Itinerary classes used for ZCPU
//===----------------------------------------------------------------------===//
def IIAlu              : InstrItinClass;


//===----------------------------------------------------------------------===//
// ZCPU Generic instruction itineraries.
//===----------------------------------------------------------------------===//
//@ http://llvm.org/docs/doxygen/html/structllvm_1_1InstrStage.html
def ZCPUGenericItineraries : ProcessorItineraries<[ALU], [], [
//@2
  InstrItinData<IIAlu              , [InstrStage<1,  [ALU]>]>
]>;
