Herald of Oblivion | Technical Showcase
Project Type: Isometric RPG

Engine: Unreal Engine 5.4+

Languages: C++, Blueprints

Architecture: Data-Driven / Type-Object Pattern

🟢 Overview
This repository is a public technical showcase of Herald of Oblivion, an isometric RPG currently under development. While the full project remains in a private repository due to proprietary assets and licensing, this version serves to demonstrate my proficiency in C++ systems architecture, Unreal Engine 5 framework, and clean code practices.

The focus here is on scalability and performance, utilizing a decoupled architecture that separates data from execution logic.

🛠️ Key Technical Features
1. Data-Driven Skill System (Type-Object Pattern)
A robust system designed to handle hundreds of unique abilities with minimal memory overhead.

UDataAsset (The "Type"): Acts as the static template for skills (base damage, cooldowns, VFX references).

UObject (The "Instance"): A lightweight persistent object for each creature, tracking dynamic data like skill level, mastery, and current cooldown state.

ASkillActor (The "Execution"): A pooled/spawned actor responsible for world representation and collision, completely decoupled from character stats.

Why this matters: This architecture allows designers to create new skills by simply creating a new Data Asset, without touching C++ code.

2. High-Performance Movement & Camera
Custom isometric camera implementation and character movement optimized for responsiveness.

Implemented via C++ to ensure frame-perfect input handling.

Calculates projection and smooth rotations independently of frame rate.

3. Professional C++ Integration
UPROPERTY/UFUNCTION Management: Strict use of memory management macros to ensure Garbage Collection safety.

Delegates & Events: Heavy use of dynamic multicast delegates for UI-to-Code communication (decoupling the HUD from the Backend).

Subsystems: Utilization of UGameInstanceSubsystem for global data management.

🎥 Video Demonstration
Since this is a code-focused repository, you can watch the systems in action here:
[Video currently unavailable.]

👤 About the Developer
Pedro Arthur Gregorio Abreu Full-stack Developer & Game Engineering Enthusiast.

Currently interning at Vitória City Hall (SUB-TI) and pursuing a B.S. in Information Technology at UniSales.

Location: Vitória, Brazil (Open to relocation to Canada/International opportunities).

Contact: pedro.agb2004@gmail.com
LinkedIn: www.linkedin.com/in/pedro-arthur-gregorio-abreu-33607021a
