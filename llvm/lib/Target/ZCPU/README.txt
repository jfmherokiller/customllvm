//===-- README.txt - Notes for ZCPU code gen -----------------------===//

This ZCPU backend is presently in a very early stage of development.
The code should build and not break anything else, but don't expect a lot more
at this point.

For more information on ZCPU itself, see the design documents:
  * https://github.com/ZCPU/design/blob/master/README.md

The following documents contain some information on the planned semantics and
binary encoding of ZCPU itself:
  * https://github.com/ZCPU/design/blob/master/AstSemantics.md
  * https://github.com/ZCPU/design/blob/master/BinaryEncoding.md

The backend is built, tested and archived on the following waterfall:
  https://wasm-stat.us

The backend's bringup is done using the GCC torture test suite first since it
doesn't require C library support. Current known failures are in
known_gcc_test_failures.txt, all other tests should pass. The waterfall will
turn red if not. Once most of these pass, further testing will use LLVM's own
test suite. The tests can be run locally using:
  https://github.com/ZCPU/waterfall/blob/master/src/compile_torture_tests.py

//===---------------------------------------------------------------------===//

Br, br_if, and br_table instructions can support having a value on the
expression stack across the jump (sometimes). We should (a) model this, and
(b) extend the stackifier to utilize it.

//===---------------------------------------------------------------------===//

The min/max operators aren't exactly a<b?a:b because of NaN and negative zero
behavior. The ARM target has the same kind of min/max instructions and has
implemented optimizations for them; we should do similar optimizations for
ZCPU.

//===---------------------------------------------------------------------===//

AArch64 runs SeparateConstOffsetFromGEPPass, followed by EarlyCSE and LICM.
Would these be useful to run for ZCPU too? Also, it has an option to
run SimplifyCFG after running the AtomicExpand pass. Would this be useful for
us too?

//===---------------------------------------------------------------------===//

Register stackification uses the EXPR_STACK physical register to impose
ordering dependencies on instructions with stack operands. This is pessimistic;
we should consider alternate ways to model stack dependencies.

//===---------------------------------------------------------------------===//

Lots of things could be done in ZCPUTargetTransformInfo.cpp. Similarly,
there are numerous optimization-related hooks that can be overridden in
ZCPUTargetLowering.

//===---------------------------------------------------------------------===//

Instead of the OptimizeReturned pass, which should consider preserving the
"returned" attribute through to MachineInstrs and extending the StoreResults
pass to do this optimization on calls too. That would also let the
ZCPUPeephole pass clean up dead defs for such calls, as it does for
stores.

//===---------------------------------------------------------------------===//

Consider implementing optimizeSelect, optimizeCompareInstr, optimizeCondBranch,
optimizeLoadInstr, and/or getMachineCombinerPatterns.

//===---------------------------------------------------------------------===//

Find a clean way to fix the problem which leads to the Shrink Wrapping pass
being run after the ZCPU PEI pass.

//===---------------------------------------------------------------------===//

When setting multiple local variables to the same constant, we currently get
code like this:

    i32.const   $4=, 0
    i32.const   $3=, 0

It could be done with a smaller encoding like this:

    i32.const   $push5=, 0
    tee_local   $push6=, $4=, $pop5
    copy_local  $3=, $pop6

//===---------------------------------------------------------------------===//

ZCPU registers are implicitly initialized to zero. Explicit zeroing is
therefore often redundant and could be optimized away.

//===---------------------------------------------------------------------===//

Small indices may use smaller encodings than large indices.
ZCPURegColoring and/or ZCPURegRenumbering should sort registers
according to their usage frequency to maximize the usage of smaller encodings.

//===---------------------------------------------------------------------===//

When the last statement in a function body computes the return value, it can
just let that value be the exit value of the outermost block, rather than
needing an explicit return operation.

//===---------------------------------------------------------------------===//

Many cases of irreducible control flow could be transformed more optimally
than via the transform in ZCPUFixIrreducibleControlFlow.cpp.

It may also be worthwhile to do transforms before register coloring,
particularly when duplicating code, to allow register coloring to be aware of
the duplication.

//===---------------------------------------------------------------------===//

ZCPURegStackify could use AliasAnalysis to reorder loads and stores more
aggressively.

//===---------------------------------------------------------------------===//

ZCPURegStackify is currently a greedy algorithm. This means that, for
example, a binary operator will stackify with its user before its operands.
However, if moving the binary operator to its user moves it to a place where
its operands can't be moved to, it would be better to leave it in place, or
perhaps move it up, so that it can stackify its operands. A binary operator
has two operands and one result, so in such cases there could be a net win by
prefering the operands.

//===---------------------------------------------------------------------===//

Instruction ordering has a significant influence on register stackification and
coloring. Consider experimenting with the MachineScheduler (enable via
enableMachineScheduler) and determine if it can be configured to schedule
instructions advantageously for this purpose.

//===---------------------------------------------------------------------===//
