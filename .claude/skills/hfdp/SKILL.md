---
name: hfdp
description: >
  Head First Design Patterns (Freeman & Robson, 2nd ed.): the nine OO design
  principles, the 14 core patterns and 9 leftover GoF patterns, how to tell
  look-alike patterns apart, and when NOT to use a pattern. Use when designing,
  reviewing, or refactoring object-oriented code, when choosing, naming, or
  questioning a design pattern, or when the user runs /hfdp or asks about
  design patterns.
---

# hfdp: Head First Design Patterns

Source: Eric Freeman & Elisabeth Robson, *Head First Design Patterns*, 2nd ed. (`learning-materials/hfdp.pdf`). The book's examples are in Java, but the ideas apply to any OO language.

## Stance: design first, patterns second

* Knowing abstraction, encapsulation, polymorphism, and inheritance doesn't make you a good OO designer. Good designs are reusable, extensible, and maintainable, and they cope with change.
* Patterns are proven OO experience. You reuse experience, not code: a pattern gives a general solution that you adapt to your problem.
* **Keep it simple.** The goal is simplicity, not "how can I apply a pattern here?" If a simpler solution works, use it.
* Introduce a pattern only when you're sure it solves a real problem in the design, and its consequences are ones you can live with.
* One exception: use a pattern even when something simpler would work if you *expect that aspect to vary*. The change must be practical and likely, not hypothetical.
* **If you don't need it now, don't do it now.** Be extensible in ways that matter. Don't add hypothetical generality.
* Refactoring is pattern time. Code full of conditionals may call for State. Concrete dependencies may call for a Factory.
* Remove a pattern when its planned flexibility isn't needed and a simpler design would be better.
* Patterns aren't free. They add classes, objects, and layers, which means complexity and sometimes inefficiency.
* Patterns are tools, not rules. Adapt them, and document how your version differs from the classic one.
* Let patterns emerge in your design. Don't force them in. The beginner wants "a pattern for Hello World". The Zen mind sees where a pattern fits naturally, reasons from principles and trade-offs, and knows how the intents of related patterns differ.

## The nine design principles

Most patterns follow from these principles. When no pattern fits, design from the principles directly. They are guidelines to strive for, not laws. Every design involves trade-offs.

1. **Encapsulate what varies.** Identify the aspects of your application that vary and separate them from what stays the same. Almost every pattern is built on this: it lets one part of the system vary independently of the rest.
2. **Program to an interface, not an implementation.** Program to a supertype (an interface or abstract class) so the concrete runtime type can change.
3. **Favor composition over inheritance.** HAS-A can be better than IS-A. Delegating to behavior objects lets behavior change at runtime. Inheriting it causes duplicated code and makes changes leak into other subclasses.
4. **Strive for loosely coupled designs between objects that interact.** Minimize how much objects depend on each other, so the design can absorb change.
5. **Open-Closed: classes should be open for extension but closed for modification.** Add behavior with new code, not by editing tested code. Apply this *only where change is likely*. Applying it everywhere is wasteful and makes code complex.
6. **Dependency Inversion: depend upon abstractions, not concrete classes.** This is stronger than #2: high-level and low-level components should *both* depend on an abstraction. To design this way, start from what you can abstract, not from the top-level class. Things to strive for:
   * No variable holds a reference to a concrete class. Use a factory instead.
   * No class derives from a concrete class.
   * No method overrides a method its base class already implements.
   * Breaking these is fine for classes unlikely to change. Instantiating `String` is fine.
7. **Least Knowledge (Law of Demeter): talk only to your immediate friends.** Inside a method, call methods only on:
   * the object itself
   * objects passed in as parameters
   * objects the method creates
   * the object's own components

   Don't call methods on objects returned by other calls. Ask your friend to make the request for you. The cost is more wrapper methods, which add complexity and runtime overhead.
8. **Hollywood: don't call us, we'll call you.** Low-level components hook into the system, and high-level components decide when and how to call them. This prevents "dependency rot", the tangle of up, down, and sideways dependencies. Avoid explicit circular dependencies between levels. Template Method, Factory Method, and Observer all use this principle.
9. **Single Responsibility: a class should have only one reason to change.** Every responsibility is an area of potential change, so give each responsibility to exactly one class. Aim for high cohesion: a class built around one set of related functions.

## The core patterns

Each entry gives the book's definition, then **Use** for when to apply it and **Watch** for trade-offs and pitfalls.

**Strategy** (behavioral): Defines a family of algorithms, encapsulates each one, and makes them interchangeable. Strategy lets the algorithm vary independently from the clients that use it.
- Use: behavior varies across subclasses or must change at runtime. The client holds a behavior interface and delegates to it, instead of inheriting the behavior. It is a flexible alternative to subclassing.

**Observer** (behavioral): Defines a one-to-many dependency between objects so that when one object changes state, all its dependents are notified and updated automatically.
- Use: objects must react to another object's changes without being tightly coupled to it. The subject knows only that observers implement the Observer interface, so observers can join or leave at any time without changing the subject.
- Watch: decide whether to push data with the notification or let observers pull it. Pull is considered more "correct". Never depend on notification order. Publish/Subscribe is the related pattern for multiple subjects or message types.

**Decorator** (structural): Attaches additional responsibilities to an object dynamically. Decorators provide a flexible alternative to subclassing for extending functionality.
- Use: you need to add behavior at runtime without modifying the class. Following Open-Closed avoids an explosion of subclasses. A decorator has the same type as the object it wraps and adds behavior before, after, or instead of delegating to it. You can stack any number of decorators.
- Watch: it produces many small classes (Java I/O is hard to read for this reason). It breaks client code that depends on the concrete component type. Building the stack of wrappers gets complex, so pair it with a Factory or Builder.

**Factory Method** (creational, class): Defines an interface for creating an object, but lets subclasses decide which class to instantiate. Factory Method lets a class defer instantiation to subclasses.
- Use: you want to decouple client code from the concrete classes it instantiates, or you don't know all the concrete classes ahead of time. Creation happens through inheritance: the abstract creator has code that *uses* the product, and each subclass implements the factory method that creates it. Factory Method is a specialization of Template Method.
- Simple Factory (one class that encapsulates creation) is a common idiom, not a pattern. It is a one-shot deal: you can't subclass it to vary the products.

**Abstract Factory** (creational): Provides an interface for creating families of related or dependent objects without specifying their concrete classes.
- Use: clients must create products that belong together. Creation happens through composition: you instantiate a concrete factory and pass it to code written against the abstract factory. Its methods are often implemented as factory methods.
- Watch: adding a new kind of product changes the interface and every concrete factory.

**Singleton** (creational): Ensures a class has only one instance, and provides a global point of access to it.
- Use: exactly one object must exist, such as a pool, registry, or device driver. Its advantage over a global variable is lazy creation. Build it with a private constructor and a static accessor.
- Watch: treat every application as multithreaded. The options are a synchronized accessor (simple but slow), eager creation, double-checked locking (`volatile`, Java 5+), or a Java `enum`, which is the simplest. Multiple class loaders can each create their own instance. Reaching for Singleton reflexively is a sign of the "intermediate mind".

**Command** (behavioral): Encapsulates a request as an object, thereby letting you parameterize other objects with different requests, queue or log requests, and support undoable operations.
- Use: you want to decouple the object that makes a request (the invoker) from the one that performs it (the receiver). A command binds a receiver to actions behind `execute()`, and invokers can be configured with commands at runtime.
- Extras: `undo()` restores the earlier state. A MacroCommand runs, and can undo, a set of commands. A null object such as `NoCommand` removes null checks. Commands also support job queues, logging, crash recovery, and transactions. In practice, "smart" commands that do the work themselves are common.

**Adapter** (structural): Converts the interface of a class into another interface the clients expect. Adapter lets classes work together that couldn't otherwise because of incompatible interfaces.
- Use: you need an existing class but its interface doesn't match. An object adapter uses composition and works anywhere. A class adapter needs multiple inheritance. The work grows with the size of the target interface.

**Facade** (structural): Provides a unified interface to a set of interfaces in a subsystem. Facade defines a higher-level interface that makes the subsystem easier to use.
- Use: you want to simplify and decouple clients from a complex subsystem. The subsystem stays directly accessible to clients that need it, and one subsystem can have several facades. Facade helps you follow Least Knowledge.

**Template Method** (behavioral, class): Defines the skeleton of an algorithm in a method, deferring some steps to subclasses. Template Method lets subclasses redefine certain steps of an algorithm without changing the algorithm's structure.
- Use: several classes share an algorithm and differ only in some steps. It reuses code and keeps the algorithm under control. Mark the template method `final`. Make a step abstract when every subclass must supply it. Make it a *hook* (an empty or default method) when the step is optional or when subclasses should be able to influence a conditional part of the algorithm.
- Watch: real-world uses often aren't "by the book". Sorting via `compareTo()` is a Template Method without subclassing.

**Iterator** (behavioral): Provides a way to access the elements of an aggregate object sequentially without exposing its underlying representation.
- Use: clients need to traverse different collections uniformly. Traversal moves out of the aggregate and into the iterator, which follows Single Responsibility and lets client code traverse polymorphically.

**Composite** (structural): Allows you to compose objects into tree structures to represent part-whole hierarchies. Composite lets clients treat individual objects and compositions of objects uniformly.
- Use: you have part-whole hierarchies such as menus, UI trees, or file systems. A Component is either a leaf or a composite.
- Watch: choose between *transparency* and *safety*. Putting child-management operations on Component keeps client code uniform but allows meaningless calls on a leaf. Separate interfaces catch those calls but force clients into type checks. Composite deliberately trades Single Responsibility for transparency.

**State** (behavioral): Allows an object to alter its behavior when its internal state changes. The object will appear to change its class.
- Use: behavior depends on state, and the code is a tangle of conditionals. Each state becomes a class, and the context delegates to its current state object. Change stays local to one state, and adding a state doesn't touch the others. Either the states or the context can own the transitions.
- Watch: it means more classes. State objects can be shared between contexts (for example as static instances) if they hold no state of their own.

**Proxy** (structural): Provides a surrogate or placeholder for another object to control access to it.
- Use: you need to control access to an object. Variants:
  * Remote: the object lives in another address space.
  * Virtual: the object is expensive to create, so the proxy stands in until it exists.
  * Protection: access depends on who is calling. Java's dynamic proxies help here.
  * Others: caching, smart reference, firewall, synchronization, copy-on-write.

  Clients often get the proxy from a factory, so they never see the real object.
- Watch: like any wrapper, a proxy adds classes and objects.

**Compound patterns / MVC.** A compound pattern combines two or more patterns into a solution that solves a recurring or general problem. MVC combines three:
- The *model* uses Observer to notify views and controllers.
- The *controller* is the view's Strategy.
- The *view* is a Composite of nested UI components.
- An Adapter can fit a new model to an existing view and controller.
- Web frameworks adapt MVC to the client/server structure.

## Leftover patterns (appendix, in brief)

| Pattern | Use when | Watch out |
|---|---|---|
| Bridge | Both the abstraction and the implementation must vary. Put them in separate hierarchies, e.g. for cross-platform UI | Adds complexity |
| Builder | A complex product (often a composite) must be built in steps while its representation stays hidden | The client needs more domain knowledge than with a factory |
| Chain of Responsibility | More than one object should get a chance to handle a request. Handlers can be added or reordered at runtime, e.g. for UI events | A request can fall off the end unhandled, and the chain is hard to debug |
| Flyweight | One instance must stand in for many identical "virtual" instances. It saves memory and centralizes their state | The logical instances can't behave independently |
| Interpreter | You need a simple language, with one class per grammar rule, and simplicity matters more than speed | Becomes cumbersome with many rules. Use a parser generator instead |
| Mediator | Communication and control among related objects should be centralized, e.g. coordinating GUI widgets | The mediator itself can become overly complex |
| Memento | An object must be returned to an earlier state (undo). The saved state lives outside the object, keeping it encapsulated | Saving and restoring can be slow |
| Prototype | Creating instances is expensive or complicated. Copy existing instances, without the client knowing the concrete class | Copying, especially deep copying, can be complicated |
| Visitor | You need to add operations to a Composite without changing it, and encapsulation is not a concern | Breaks encapsulation and makes structural changes harder |

## Telling look-alikes apart

When patterns look alike, intent is what tells them apart.
- **Strategy vs State:** same class diagram. With Strategy, the client *configures* the context with a behavior. With State, the context *changes its own* behavior as its internal state changes, replacing conditionals.
- **Strategy vs Template Method:** both encapsulate algorithms. Strategy uses composition and swaps the whole algorithm. Template Method uses inheritance, swaps individual steps, and keeps control of the algorithm's structure.
- **The wrappers:** Adapter *changes* an interface. Decorator *adds* behavior and keeps the interface. Proxy *controls access* and keeps the interface. Facade *simplifies* a whole set of objects.
- **The factories:** Simple Factory is an idiom: one place for creation. Factory Method uses inheritance to make one product and lets the subclass decide. Abstract Factory uses composition to make families of products.
- **The principles:** Program-to-interface says use supertypes. Dependency Inversion is stronger: both levels depend on abstractions. Hollywood is a framework technique that lets low-level components hook in without depending on higher levels.

## Classifying, choosing, and documenting

- **Definition:** a pattern is a solution to a problem in a context. The *context* is a recurring situation. The *problem* is a goal plus constraints, which together are called the "forces". The *solution* is a general design that balances those forces.
- A pattern also needs a name, so it becomes shared vocabulary. Patterns are discovered, not invented. Rule of Three: it isn't a pattern until it has been used in three real-world solutions. Most "new" patterns turn out to be variants of existing ones.
- **Categories:** use them when they help. Knowing the patterns and how they relate matters more.
  * *Creational* patterns decouple the client from what it instantiates: Singleton, Factory Method, Abstract Factory, Builder, Prototype.
  * *Structural* patterns compose classes or objects into larger structures: Adapter, Bridge, Composite, Decorator, Facade, Flyweight, Proxy.
  * *Behavioral* patterns cover how objects interact and divide responsibility: Chain of Responsibility, Command, Interpreter, Iterator, Mediator, Memento, Observer, State, Strategy, Template Method, Visitor.
  * *Class* patterns set relationships through inheritance at compile time: Template Method, Factory Method, class Adapter, Interpreter. All the others are *object* patterns, set through composition at runtime.
- **Choosing a pattern from a catalog:**
  1. Learn the patterns and how they relate before you need them.
  2. Match your problem and its constraints against each pattern's Intent and Applicability sections.
  3. Read Consequences to check for unwanted effects on your design.
  4. Read Structure and Participants to understand each role, then fit the pattern to your design.
  5. Read Implementation and Sample Code for techniques and gotchas.
- **Anti-pattern:** a recurring way from a problem to a *bad* solution. A good write-up explains why the bad solution is tempting, why it hurts in the long run, and which good patterns to use instead. Example: the *Golden Hammer*, where the team's one familiar technology is applied obsessively, even where it clearly doesn't fit. The fix is to broaden the team's knowledge.
- **Share the vocabulary:**
  * Use pattern names in design talks, to stay "in the design" rather than in implementation details.
  * Use them in architecture docs.
  * Name the patterns in code comments, and choose class and method names that reveal the pattern underneath.

## Applying hfdp

When you design or review code with this skill:
1. Find what varies and what stays the same, and encapsulate what varies.
2. Check the nine principles: interfaces over implementations, composition over inheritance, loose coupling, Open-Closed only where change is likely, abstractions over concretes, talk only to friends, high-level components call low-level ones, one reason to change per class.
3. Use a pattern only when it addresses a real or likely problem. Name the problem, the pattern, and the consequences you accept.
4. Prefer the simplest design that meets today's needs. Point out any pattern that adds complexity without practical flexibility, and suggest removing it.
5. When patterns look alike, compare their *intent*. When you adapt a pattern, document how it differs from the classic.
