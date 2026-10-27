---
name: p10
description: >
  The Power of Ten: Gerard Holzmann's (NASA/JPL) ten rules for safety-critical
  code. They keep control flow simple, bound every loop, allocate no memory
  after init, keep functions short, require dense assertions, minimal scope,
  checked returns and parameters, a restricted preprocessor, and restricted
  pointers, and demand zero warnings. Use when writing, reviewing, or
  refactoring code that must be reliable or statically verifiable, or when the
  user runs /p10 or asks for the Power of Ten rules.
---

# p10: The Power of Ten

Source: Gerard J. Holzmann, *The Power of Ten: Rules for Developing Safety
Critical Code*, NASA/JPL Laboratory for Reliable Software
(`learning-materials/P10.pdf`).

## Why only ten rules

* Long coding standards get ignored, so this one is kept small.
* A rule is only worth having if a tool can check it mechanically.
* The rules are strict on purpose. In return, they let you *prove* things
  about the code (it terminates, its memory is bounded, its stack is bounded)
  instead of hoping.
* They target C, but the reasoning carries over to other languages. See
  [Applying beyond C](#applying-beyond-c).

## The Ten Rules

### 1. Simple control flow

* No `goto`, `setjmp`, or `longjmp`.
* No recursion, direct or indirect.
* Early error returns are fine. A single exit point is **not** required.

**Why:** With no recursion, the call graph is acyclic. Analyzers can use that
to prove that execution is bounded.

### 2. Every loop has a fixed upper bound

* A checker must be able to prove statically that the loop cannot run past a
  preset bound. If it can't, the rule is broken.
* If a loop has a variable trip count (walking a linked list, reading a
  stream), add an explicit bound. When the bound is exceeded, the assertion
  fails and the function returns an error (see Rule 5).
* Exception: a loop *meant* to run forever (a scheduler, a main event loop).
  Here the rule is reversed: it must be provable that the loop *cannot*
  terminate.

**Why:** No recursion plus bounded loops means no runaway code.

```c
for (int i = 0; node != NULL && i < MAX_NODES; i++) { ... }
if (!c_assert(node == NULL)) { return ERROR; }   /* bound was hit */
```

### 3. No dynamic memory allocation after initialization

* Allocate everything up front. No `malloc`/`free` and no GC-driven
  allocation once the system is running.
* Stack memory is the only dynamic memory left. Because Rule 1 rules out
  recursion, stack depth can be bounded statically.

**Why:** Allocators and garbage collectors behave unpredictably. This rule
also removes a whole class of bugs: leaks, use-after-free, over-allocation,
and buffer overruns on heap blocks.

### 4. Functions fit on one printed page

* About **60 lines** at most: one line per statement and one per declaration.
* A function is one logical unit that can be understood and verified on its
  own.

**Why:** A unit spread across several screens is hard to understand. Long
functions usually mean poor structure.

### 5. At least two assertions per function, on average

* Assertions check for anomalous conditions that should never happen in a
  real run: pre-conditions, post-conditions, parameter values, return values,
  and loop invariants.
* They must be **side-effect free** Boolean tests.
* A failed assertion must trigger an **explicit recovery action**, usually
  returning an error to the caller. It must not just log the failure and carry
  on.
* An assertion that a static checker can prove always holds or always fails
  breaks the rule. `assert(true)` padding doesn't count.

**Why:** Unit tests find about 1 defect per 10–100 lines. The denser the
assertions, the better the odds of catching one. Because assertions have no
side effects, they can be disabled in performance-critical code after testing.

```c
#define c_assert(e) ((e) ? (true) : \
    (tst_debugging("%s,%d: assertion '%s' failed\n", \
     __FILE__, __LINE__, #e), false))

if (!c_assert(p >= 0) == true) {
    return ERROR;
}
```

On embedded targets, `tst_debugging` becomes a no-op, leaving a pure Boolean
test that drives recovery.

### 6. Declare data at the smallest possible scope

* Don't reuse one variable for several unrelated purposes.

**Why:** Data hiding. An object that is out of scope can't be read or
corrupted. The fewer places that can assign a value, the easier a bad value is
to diagnose.

### 7. Check every return value and every parameter

* Every caller checks the return value of every non-void function.
* Every function checks that its parameters are valid.
* Ignore a return value only when handling an error would look exactly like
  handling success (`printf`, `close`). In C, mark that by casting to
  `(void)`, so the choice is clearly deliberate. In more dubious cases, add a
  comment explaining why the value doesn't matter.
* Propagate errors up the call chain. Don't swallow them.

**Why:** This is the most frequently violated rule. Keeping it as the default
forces every exception to be justified, and checkers flag the rest. Complying
is usually easier than explaining why you didn't.

### 8. Restrict the preprocessor

* Use it only for header includes and simple macro definitions.
* No token pasting, no variadic macros (`...`), no recursive macros.
* Every macro must expand to a complete syntactic unit.
* Keep conditional compilation to one or two directives across the whole
  project, beyond standard include guards. Flag each use with a checker and
  justify it in the code.

**Why:** The preprocessor obscures code and confuses text-based checkers. Ten
`#if`s can produce up to 2¹⁰ versions of the code, and each one needs testing.

### 9. Restrict pointers

* No more than **one level** of dereference.
* Don't hide dereferences inside macros or `typedef`s.
* No function pointers. If one is truly unavoidable, give the checker another
  way to establish control flow and the call hierarchy. A function pointer can
  make it impossible to prove there is no recursion, so make up that guarantee
  some other way.

**Why:** Pointers are easy to misuse and hard for analyzers to follow.
Function pointers hide the call graph.

### 10. Zero warnings, from day one

* Compile with **all** warnings enabled at the compiler's most pedantic
  setting, starting on the first day.
* Keep the code warning-free under those settings.
* Run at least one, and preferably several, state-of-the-art static analyzers
  every day, and keep them at **zero warnings**.
* If a warning is wrong, rewrite the code until it is *trivially* valid.
  Don't suppress the warning. Many warnings that looked invalid turned out to
  be valid for less obvious reasons.

**Why:** The tools exist and are cheap, so there is no excuse not to use them.
This is routine practice even for code that isn't critical.

## How the rules fit together

* **Rules 1–2:** clear, transparent control flow that is easy to build, test,
  and analyze.
* **Rule 3:** removes allocation and free errors and stray pointers.
* **Rules 4–7:** widely accepted good style. Rules 5–7 together give you most
  of what design-by-contract offers.
* **Rules 8–10:** keep the code readable to tools, so the other rules can be
  checked mechanically.

## Applying beyond C

The paper is about C. In other languages, keep the intent of each rule:

| Rule | Intent to keep |
|------|----------------|
| 1 | No recursion. Prefer explicit loops and early returns. Don't use exceptions as hidden `goto`s for control flow. |
| 2 | Every `while`, iterator, or retry loop has a provable maximum. Assert when it is reached. |
| 3 | In managed languages, preallocate buffers and pools and avoid allocating in steady-state and hot paths. Treat this as strict only where the target requires it. |
| 5 | Use real runtime checks that return errors or results, not just debug-only `assert`s that disappear in release builds. |
| 7 | Handle every `Result`, error, or promise. Discard one only explicitly (`_ =`, `void`) and say why. |
| 8 | Avoid macros, code generation, and build flags that create many code variants. Limit feature flags and `#if`s. |
| 9 | Limit indirection: avoid deep reference chains, dynamic dispatch through stored callbacks, and reflection, because they hide the call graph. |
| 10 | Strictest compiler and linter settings, type checker in strict mode, analyzers in CI, zero warnings, no blanket suppressions. |

## Review checklist

When writing or reviewing code under p10, check each item:

1. [ ] No `goto`, `setjmp`, or `longjmp`, and no recursion (direct or through
   other functions).
2. [ ] Every loop has a provable bound, or is provably non-terminating by
   design.
3. [ ] No allocation after initialization.
4. [ ] Every function is ≤ ~60 lines.
5. [ ] Assertions average ≥ 2 per function. They have no side effects, are not
   trivially always true or always false, and failure triggers recovery.
6. [ ] Every variable is declared at its smallest scope, with one purpose per
   variable.
7. [ ] Every non-void return value is checked or explicitly discarded with a
   reason, and every parameter is validated.
8. [ ] Preprocessor use is limited to includes, simple whole-unit macros, and
   justified conditionals.
9. [ ] At most one level of dereference, no hidden dereferences, no function
   pointers.
10. [ ] Zero warnings at the most pedantic compiler settings and from the
    static analyzers.

When a rule is broken, name the rule number, the line, and the fix. If you
break a rule on purpose, justify it in the code where a checker can flag it.

> The rules act like a seat-belt: a little uncomfortable at first, then second
> nature, and eventually going without them is unimaginable.
