---
name: cc
description: >
  Clean Code (Robert C. Martin et al.): language-agnostic practices for code that reads well and stays cheap to
  change. Covers meaningful names, small one-thing functions, few honest comments, formatting, objects vs. data
  structures, error handling, boundaries, clean tests, small cohesive classes, system-level separation of concerns,
  simple design, concurrency, and the smells-and-heuristics catalog. Use when writing, reviewing, or refactoring
  code for readability and maintainability, or when the user runs /cc or asks about clean code.
---

# cc: Clean Code

Source: Robert C. Martin et al., *Clean Code: A Handbook of Agile Software Craftsmanship* (`learning-materials/cc.pdf`).
The book's examples are Java, but these practices apply in any language. Tags like [G5] are the book's Chapter 17
heuristic IDs. Cite them in reviews.

## Stance

* Code is read over 10x more than it is written. Optimize for the reader, because easy to read is easy to write.
* The only way to go fast is to keep the code clean at all times. A mess slows you down today, and "later equals never."
* Clean code does one thing well, reads like prose, and has tests. Each routine is "pretty much what you expected."
* Boy Scout Rule: check each module in a little cleaner than you found it. One better name, one split function.
* Defend the code with the same passion managers defend the schedule. Making a mess on request is unprofessional.
* This is one school of thought, not law. Values that drive disciplines make a craftsman. Rule lists alone do not.

## Names

* Reveal intent: why it exists, what it does, how it is used. If a name needs a comment, rename it. [N1]
* No disinformation (`accountList` that isn't a list, look-alike names, a lone `l`/`O`) and no noise (`a1`, `Info`).
* Pronounceable, searchable names. Length grows with scope: `i` in a 3-line loop, precise names in wide scopes. [N5]
* No encodings: no type prefixes or Hungarian notation, no `m_`, no `I` on interfaces, no project prefixes. [N6]
* No mental mapping and no cuteness (`whack()`, `HolyHandGrenade`). Clarity is king. Say what you mean.
* Types get nouns (`Customer`, `AddressParser`), not `Manager` or `Processor`. Functions get verbs (`postPayment`).
* Use named factories that describe their arguments (`Complex.fromRealNumber(23.0)`) instead of overloaded constructors.
* One word per concept (not `fetch`, `retrieve` and `get` side by side). Don't pun: one word never means two things.
* Use solution-domain terms (patterns, algorithms, CS) for technical ideas and problem-domain terms otherwise. [N3]
* Give context by putting names inside well-named types or modules. Add no gratuitous context (`GSDAccountAddress`).
* Name at the level of abstraction, not the implementation: `connect(locator)`, not `dial(phoneNumber)`. [N2]
* Names must be unambiguous and must mention side effects (`createOrReturnStream`, not `getStream`). [N4][N7]
* Rename whenever you find a better name. Hunting for a good name often reveals a better structure.

## Functions

* Small, then smaller: a few lines, hardly ever 20. Each `if`/`while` block is one call. Nest at most two levels.
* Do one thing: every step is one level of abstraction below the function's name. If you can extract a function whose
  name isn't a restatement of its code, it did more than one thing. Internal sections are a symptom too. [G30]
* Keep one level of abstraction per function. Don't mix `renderPage()` with `append("\n")`. [G34]
* Stepdown rule: the file reads top-down as "TO do X, we do A, then B" paragraphs, each caller followed by its callees.
* Switch/if-chains on a type are allowed once per selection, buried in a factory that creates polymorphic objects. [G23]
* Arguments: zero is ideal, then one, then two. Three needs strong justification. More than three, don't. [F1]
  * A one-argument function asks a question (`fileExists(f)`), transforms and returns (`open(f)`), or is an event.
  * No flag or selector arguments. Split the function (`renderForSuite()`, `renderForSingleTest()`). [F3][G15]
  * No output arguments. Return the result, or change the state of the receiver. [F2]
  * Arguments that travel together are a concept, so make them a type. Let names encode argument order.
* No hidden side effects. A `checkPassword` that also starts a session lies and creates temporal coupling.
* Command-query separation: a function either changes state or answers a question, never both.
* Prefer exceptions (or the language's native error mechanism) to error codes, which nest checks at every call site.
* Error handling is one thing: extract try/catch bodies into functions, and put nothing before `try` or after `catch`.
* DRY: duplication may be the root of all evil in software. Fold it into a function, a type, or polymorphism. [G5]
* In small functions, early `return`/`break`/`continue` are fine. Single-exit rules only pay off in large functions.

## Comments

* A comment marks a failure to express yourself in code. Comments rot and lie. Only the code tells the truth.
* Don't comment bad code: rewrite it. Replace an explaining comment with a well-named function or variable.
* Acceptable: legal headers (point to the license), intent or rationale, clarifying a value you can't change,
  warnings of consequences, TODOs (sweep them regularly), amplifying something that looks trivial, public API docs.
* Bad: mumbling, restating the code [C3], misleading text, mandated docs on every function or variable, change logs and
  bylines (version control keeps these) [C1], noise, banners, closing-brace comments, markup, info about distant code,
  history lessons, comments whose link to the code isn't obvious, formal doc comments on non-public code.
* Delete commented-out code and fix or delete obsolete comments. Write the rest well: brief and precise. [C2][C4][C5]

## Formatting

* Formatting is communication. The team agrees on rules once, encodes them in a formatter, and everyone follows them.
* Small files (about 200 lines, rarely over 500). The top tells the high-level story, and detail grows downward.
* Blank lines separate concepts. Tightly related lines stay dense, with no noise between them.
* Keep related things vertically close. Declare locals near first use and fields in one known place.
  Put each caller just above its callee, and keep look-alike functions together. [G10]
* Keep lines under ~120 chars. Use horizontal space for association and precedence (`b*b - 4*a*c`); don't align columns.
* Always indent scopes, even one-liners. Put an empty loop body on its own visible line.

## Objects and data structures

* Objects hide data behind abstractions and expose behavior. Data structures expose data and have no real behavior.
* Don't add getters and setters by reflex. Express data abstractly (`percentFuelRemaining()`, not raw gallons).
* Procedural code makes new functions easy and new types hard. OO makes new types easy and new functions hard.
  Choose per area of the system. Not everything is an object.
* Avoid hybrids that are half object and half data structure. Keep business rules out of DTOs and active records.
* Law of Demeter: call methods only on self, your arguments, objects you create, and your own fields. No train wrecks
  (`a.getB().getC().do()`). Tell the object to do the job instead. Plain data structures are exempt. [G36]

## Error handling

* Error handling matters, but if it obscures the logic it is wrong. Keep it separate from the happy path.
* Write the try/catch/finally first, from a test that forces the error. It's a transaction: leave state consistent.
* Don't make each layer between thrower and catcher declare low-level errors. Checked exceptions break encapsulation.
* Give errors context (what failed, how). Define error types by how callers catch them; wrap vendor APIs to raise them.
* For an expected case, return a Special Case object that handles it instead of throwing.
* Don't return null: throw, return a Special Case, or return an empty collection. Don't pass null. Forbid it by default.

## Boundaries

* Don't pass boundary types (generic maps, vendor objects) around the system. Wrap them in a type built for your needs.
* Learn a third-party API by writing learning tests, and rerun them on every upgrade. Boundary tests ease migration.
* For an API that doesn't exist yet, write the interface you wish you had; add an Adapter later. It's also a test seam.
* Few places should know third-party particulars. Depend on what you control, or it ends up controlling you.

## Unit tests

* Three Laws of TDD: no production code without a failing test, only enough test to fail (not compiling counts as
  failing), and only enough code to pass.
* Test code is as important as production code. Tests make change safe. Dirty tests get dropped, and then the code rots.
* Readability above all: build-operate-check (given/when/then) and a domain-specific testing language of helpers.
* Dual standard: tests may be less efficient than production code, never less clean.
* Test one concept per test and keep asserts per concept to a minimum.
* F.I.R.S.T.: Fast, Independent, Repeatable anywhere, Self-validating (pass/fail), Timely (just before the code). [T9]
* Test all that could break; use coverage; don't skip trivial tests. A disabled test is an open question. [T1]–[T4]
* Test boundaries; test exhaustively near bugs (they cluster). Failure and coverage patterns point to causes. [T5]–[T8]

## Classes

* Order members: constants, static state, fields, then each public function followed by its private helpers.
  Keep everything private. Loosen encapsulation (for example, for a test) only as a last resort.
* Measure size in responsibilities. A class you can't name concisely, or describe in about 25 words without
  "if/and/or/but", is too big. `Manager`, `Processor` and `Super` are warning signs.
* SRP: one reason to change per class. Prefer many small classes to a few large ones (labeled drawers beat a junk box).
* Cohesion: few fields, each used by most methods. When some methods share some fields, split them into a class.
* OCP: add features by adding new classes, not by editing tested ones. Restructure when change actually arrives.
* DIP: depend on abstractions, not concrete details. This isolates change and lets tests substitute stubs.

## Systems

* Separate construction from use. Build and wire objects at startup (main, factories, dependency injection), and let
  the rest of the code assume they exist. Scattered lazy-init with hard-coded concrete types breaks modularity.
* Dependencies point away from main. Use an abstract factory when the application must control when something is made.
* Separated concerns let architecture grow incrementally: build today's stories, refactor, skip big design up front.
* Keep domain logic in plain, framework-free objects. Add cross-cutting concerns with decorators, proxies, or aspects.
* Postpone decisions to the last possible moment. Adopt a standard only when it adds demonstrable value.
* Use domain-specific languages or APIs so code reads like the domain. Use the simplest thing that can possibly work.

## Simple design (Kent Beck's four rules, in priority order)

1. **Runs all the tests.** Making the system testable pushes it toward small, decoupled units.
2. **Contains no duplication** of code or implementation (`isEmpty()` → `size() == 0`). Use Template Method/Strategy.
3. **Expresses the programmer's intent** through names, small units, standard pattern names, and tests as documentation.
4. **Minimizes the number of classes and functions.** Lowest priority. Resist dogma such as an interface per class.

## Concurrency

* Concurrency decouples *what* from *when*. It pays off for I/O-bound work, costs overhead, and changes the design.
* SRP: keep concurrency code apart from other code. Thread management lives in a few places and does nothing else.
* Limit the scope of shared data, prefer copies, and partition data so threads stay independent.
* Know your library: thread-safe collections, executors/pools, nonblocking compare-and-swap atomics, and what is not
  thread-safe. A check-then-act sequence on a thread-safe object is still not thread-safe.
* Know the classic models (producer-consumer, readers-writers, dining philosophers) and starvation, deadlock, livelock.
* Avoid calling several locked methods of one shared object in sequence. If you must, prefer server-based locking.
* Keep critical sections as small as possible, and don't lock what isn't shared.
* Deadlock needs four conditions: mutual exclusion, lock-and-wait, no preemption, and circular wait. Break one. Global
  lock ordering is the usual fix.
* Design shutdown early and get it working early. It takes longer than you expect.
* Testing: treat every spurious failure as a threading bug, because one-offs don't exist. Get the non-threaded code
  working first. Make the threaded code pluggable and tunable. Run more threads than cores, on every target platform,
  and jiggle (instrument) the code to force rare interleavings.

## General heuristics

* Build with one command, and run all tests with one command. [E1][E2]
* Delete dead code and clutter (unused variables, empty constructors, noise). One language per file. [G9][F4][G12][G1]
* Implement the obvious behavior. Test every boundary and corner case instead of trusting intuition. [G2][G3]
* Don't override safeties: no silenced warnings, no disabled failing tests. [G4]
* Put code at its right level of abstraction. Base classes never know their derivatives. [G6][G7]
* Keep interfaces small. Hide data, utilities, constants, and temporaries. [G8]
* Be consistent: do similar things the same way. Don't be arbitrary: let structure show its reason. [G11][G32]
* No artificial coupling: put general things in general places. Avoid feature envy. [G13][G14]
* Make intent visible: name your constants (`SECONDS_PER_DAY`), use explanatory variables, and give functions names that
  say what they do (`plusDays(5)`, not `add(5)`). [G16][G19][G20][G25]
* Put code where readers expect it. Prefer instance methods to static unless polymorphism is never wanted. [G17][G18]
* Understand the algorithm. Passing tests isn't enough, so refactor until correctness is obvious. [G21]
* Make logical dependencies physical: ask the collaborator (`formatter.maxPageSize()`) instead of assuming. [G22]
* Follow team and industry conventions, but enforce key decisions with structure (types, abstract methods). [G24][G27]
* Be precise: no floats for money; check for null and extra matches; lock if updates can be concurrent. [G26]
* Name conditionals (`shouldBeDeleted(t)`) and boundary math (`nextLevel`); prefer positive conditions. [G28][G29][G33]
* Expose temporal coupling with a "bucket brigade": each step returns what the next step needs. [G31]
* Keep configuration and defaults at high levels and pass them down. [G35]
* Prefer enums with behavior to bare int constants. Don't inherit constants to get around scoping. [J2][J3]

## Successive refinement: how to get there

* Write dirty code first, then clean it; working isn't done. Refactor as soon as the structure starts to rot.
* Never make big "improvement" rewrites. Make many tiny changes and keep every test passing after each one.
* With legacy code, pin its behavior with tests first (raise coverage), fix the bugs they expose, then make it right.
* Refactoring loops: later steps sometimes undo earlier ones. Treat code critique as normal professional review.

## Applying

When writing, follow the rules above as you refine each draft. When reviewing, give the tag ([G23], [N7]), the line, and
the fix for each finding. When a rule is broken on purpose, say why in the code or in the change description.
