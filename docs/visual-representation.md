# Mobile Drone-in-a-Box — Visual Representation

```mermaid
flowchart TD

    A["PROJECT CONTEXT<br/>Mobile Drone-in-a-Box system<br/>mounted on a vehicle<br/><br/>Goal: develop and demonstrate a safe,<br/>reliable and robust working proof of concept"]

    %% =====================================================
    %% PHASE 1 - DIAGNOSIS
    %% =====================================================
    subgraph P1["PHASE 1 · DIAGNOSIS — What is the current situation and what still needs to be solved?"]

        B1["Literature and Desk Research<br/>• Existing drone-in-a-box systems<br/>• Suitable European small drones<br/>• Opening and closing mechanisms<br/>• Take-off and landing requirements<br/>• Battery and charging possibilities<br/>• Wired and remote-control solutions"]

        B2["Stakeholder and Requirements Research<br/>• Discuss needs with the client<br/>• Investigate practical use cases<br/>• Define operating environment<br/>• Determine safety and reliability expectations<br/>• Translate needs into system requirements"]

        B3["Benchmark and Competitor Analysis<br/>• Stationary drone-in-a-box systems<br/>• Existing mobile drone systems<br/>• Mechanical opening and closing systems<br/>• Drone-centering solutions<br/>• Transport-locking solutions<br/>• Charging / wireless charging solutions"]

        B4["Technical and Environmental Analysis<br/>• Vehicle movement and vibration<br/>• Weather exposure<br/>• Roof mounting constraints<br/>• Take-off and landing clearance<br/>• Available vehicle power<br/>• Mechanical, electrical and software interfaces"]

        B5["CURRENT PROJECT STATUS<br/>Already available:<br/>• Opening/closing concept<br/>• 3D mechanical design<br/>• Scissor mechanism nearly completed<br/><br/>Still missing:<br/>• Electronics and sensors<br/>• Actuator control and control software<br/>• Drone selection<br/>• Centering mechanism<br/>• Transport locking<br/>• Remote / wired control<br/>• Charging investigation<br/>• Full system integration and testing"]

        B6["PROBLEM DIAGNOSIS<br/>The mechanical opening concept is already largely defined,<br/>but a complete mobile drone-in-a-box system does not yet exist.<br/><br/>The remaining challenge is to integrate the drone, mechanics,<br/>electronics and software into one system that can safely deploy,<br/>recover, secure and transport the drone from a moving vehicle."]

        B1 --> B6
        B2 --> B6
        B3 --> B6
        B4 --> B6
        B5 --> B6
    end

    %% =====================================================
    %% PHASE 2 - OBJECTIVES
    %% =====================================================
    subgraph P2["PHASE 2 · OBJECTIVES — What must the project achieve?"]

        C1["Project Planning and Requirements<br/>• Create project plan<br/>• Create requirements document<br/>• Define functional requirements<br/>• Define non-functional requirements<br/>• Define measurable acceptance criteria"]

        C2["Drone Selection<br/>Investigate and select a suitable<br/>European small drone based on:<br/>• Dimensions and weight<br/>• Flight capability<br/>• Landing requirements<br/>• Battery system<br/>• Integration possibilities"]

        C3["Mechanical System<br/>Complete and validate:<br/>• Scissor mechanism<br/>• Roof mounting<br/>• Opening / closing movement<br/>• Mechanical safety<br/>• Structural robustness"]

        C4["Electronics and Control<br/>Develop:<br/>• Sensors<br/>• Actuators and drivers<br/>• Control electronics<br/>• Opening / closing logic<br/>• Safety interlocks"]

        C5["Control Interface<br/>Investigate and implement:<br/>• Wired control<br/>• Remote control<br/>• Manual override<br/>• System status feedback"]

        C6["Drone Centering and Transport Lock<br/>Design a robust system that:<br/>• Guides the drone after landing<br/>• Centers the drone correctly<br/>• Prevents movement during transport<br/>• Allows safe deployment afterwards"]

        C7["Power and Charging<br/>Investigate:<br/>• Power from vehicle battery<br/>• Voltage / power conversion<br/>• Drone charging requirements<br/>• Wireless charging feasibility<br/><br/>Optional if project time allows"]

        C8["MAIN PROJECT OBJECTIVE<br/>Build, integrate, test and demonstrate a working<br/>proof of concept of the mobile Drone-in-a-Box system."]

        C1 --> C8
        C2 --> C8
        C3 --> C8
        C4 --> C8
        C5 --> C8
        C6 --> C8
        C7 -. Optional .-> C8
    end

    %% =====================================================
    %% PHASE 3 - SOLUTION DIRECTION
    %% =====================================================
    subgraph P3["PHASE 3 · SOLUTION DIRECTION — How will the objectives be achieved?"]

        D1["System Architecture<br/>Define how the subsystems interact:<br/>Drone - Mechanical system - Electronics -<br/>Sensors - Control software - Vehicle"]

        D2["Mechanical Development<br/>Finalize existing scissor mechanism<br/>and integrate enclosure, roof mounting,<br/>centering and transport locking"]

        D3["Electronics Development<br/>Select and integrate:<br/>• Actuators<br/>• Position sensors<br/>• Limit switches<br/>• Controllers and drivers<br/>• Power electronics"]

        D4["Software Development<br/>Implement control logic for:<br/>• Open box<br/>• Confirm safe position<br/>• Allow take-off<br/>• Detect / confirm landing<br/>• Close box<br/>• Detect faults<br/>• Manual / remote operation"]

        D5["Drone Integration<br/>Validate selected drone with:<br/>• Enclosure dimensions<br/>• Take-off and landing<br/>• Centering system<br/>• Locking mechanism<br/>• Control requirements<br/>• Power / charging requirements"]

        D6["Prototype Integration<br/>Combine mechanical, electrical and<br/>software subsystems into one prototype"]

        D7["Testing and Validation<br/>Test against requirements:<br/>• Opening / closing<br/>• Take-off / landing<br/>• Centering<br/>• Transport locking<br/>• Wired / remote control<br/>• Safety and reliability<br/>• Vehicle vibration<br/>• Environmental conditions"]

        D8["FINAL OUTPUT<br/>Working proof of concept<br/>• Project plan<br/>• Requirements document<br/>• Design document<br/>• Test report<br/>• Final project document<br/>• Demonstration"]

        D1 --> D2
        D1 --> D3
        D1 --> D4
        D1 --> D5

        D2 --> D6
        D3 --> D6
        D4 --> D6
        D5 --> D6

        D6 --> D7
        D7 --> D8
    end

    %% =====================================================
    %% MAIN PROJECT JOURNEY
    %% =====================================================
    A --> P1
    B6 --> P2
    C8 --> P3

    %% =====================================================
    %% STYLING
    %% =====================================================
    classDef context fill:#e8eaf6,stroke:#3949ab,stroke-width:2px
    classDef milestone fill:#fff8e1,stroke:#f9a825,stroke-width:2px
    classDef output fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    classDef optional fill:#fafafa,stroke:#9e9e9e,stroke-dasharray:4 3

    class A context
    class B6,C8 milestone
    class B5 context
    class D8 output
    class C7 optional
```
