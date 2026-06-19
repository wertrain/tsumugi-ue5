# Tsumugi

A lightweight scripting engine for narrative-driven games.

Tsumugi is a custom scripting language and runtime designed for building
story-driven applications such as visual novels and adventure games.

---

## 🎬 Demo

A simple example demonstrating **class inheritance** in Tsumugi Script.

![demo](Documents/Demo/output.gif)

```ts
class animal {
    init(name) {
        this.name = name
    }

    speak() {
        return this.name + " makes a sound"
    }
}

class dog extends animal {
    init(name) {
        this.name = name
    }

    speak() {
        return this.name + " barks"
    }
}

let d = dog("pochi")
d.speak()
```
```
Output:
    > "pochi barks"
```
---

## 🎬 Visual Novel Demo
```
[image storage="sample\\image\\bg_room.png" layer="base"]
[image storage="sample\\image\\char_idle.png" layer=0 left=200 top=100 visible=true opacity=255]
[font color=0xFF4444]
"This is the main character's room."
```

## ✨ Features

- 🧠 Custom scripting language (class, inheritance, functions)
- 🪶 Lightweight and easy to embed
- 🎮 Designed for narrative-driven games
- 🧩 Tag-based text system for scenario scripting
- 🧱 Modular architecture (engine / runtime / integrations)
- 🛠 Unreal Engine integration (via plugin)

---

## 🧩 Architecture

Tsumugi is composed of several components:

    Script → Engine → Runtime → Application / Game

### Core Components

- Tsumugi Script Engine
  - Lexer / Parser
  - AST
  - Evaluator
  - Object system

- Runtime
  - Text rendering
  - Input handling
  - Visual novel-style features (example implementation)

- Integrations
  - Unreal Engine plugin

---

## 🎯 Use Cases

Tsumugi is designed for:

- Visual novel engines
- Adventure games
- Dialogue systems
- Narrative scripting tools
- Lightweight embedded scripting

---

## 🚀 Getting Started

You can quickly try Tsumugi using the REPL:

```
Hello Tsumugi Script!
>> let fib = function(n) { if(n == 0) { return 0; } if(n == 1) { return 1;} else { fib(n-1) + fib(n-2) }
>> fib(20);
6765
>>
```

---

## 💡 Why Tsumugi?

There are many scripting systems, but Tsumugi focuses on:

- Simplicity — minimal setup, easy to understand  
- Portability — embeddable in various environments  
- Game-oriented design — built specifically for narrative systems  

Unlike large frameworks, Tsumugi is designed to be:

    small, hackable, and easy to integrate into your own engine

---

## 🔌 Unreal Engine Integration

Tsumugi can be integrated into Unreal Engine via plugin.

- Execute scripts inside UE
- Drive dialogue systems
- Combine with gameplay logic

---

## 📁 Project Structure

The repository follows a development-oriented structure with Unreal Engine integration.

    Libraries/TsumugiLib/Source/Tsumugi/
        Core scripting engine (lexer, parser, evaluator, runtime)

    Libraries/TsumugiLib/Source/TsumugiEditor/
        Unreal Engine editor integration

    TsumugiGame/
        Example application (visual novel style runtime)

    Tests/
        Test cases and sample scripts

    Documents/
        Additional documentation

---

## 📌 Status

This project is actively being developed.

---

## 📄 License

MIT License
