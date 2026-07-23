# 🏢 Elevator Simulation System

[![GitHub Repository](https://img.shields.io/badge/GitHub-madhav7802-blue.svg)](https://github.com/madhav7802/vs-code-projects)
<!-- Add other badges here like build status or license -->

## 📌 Overview
This project is a dynamic Elevator Simulation system designed to model real-world elevator dispatching and request handling. It demonstrates core principles of system design, object-oriented programming (OOP), and algorithmic efficiency by managing concurrent passenger requests across multiple floors and elevator cars.

## ✨ Key Features
*   **Smart Dispatching:** Utilizes a custom scheduling algorithm (e.g., SCAN/Elevator algorithm, FCFS, or Shortest Seek Time First) to minimize passenger wait times.
*   **State Management:** Accurately tracks the state of each elevator (Idle, Moving Up, Moving Down, Doors Open/Closed).
*   **Concurrent Request Handling:** Simulates multiple internal (inside the cabin) and external (hallway) button presses simultaneously.
*   **Extensible Architecture:** Designed with modularity in mind, allowing for easy adjustment of floor counts, elevator capacity, and the number of active cars.

## 🛠️ Tech Stack
*   **Language:** [e.g., C++, Python, Java] 
*   **Core Concepts:** Object-Oriented Design, Data Structures (Queues, Priority Queues), Event-Driven Simulation.

## 🧠 System Architecture & Algorithms

### The Dispatching Algorithm
*Briefly explain the logic your elevator uses to decide where to go next. For example:*
The simulation employs the **SCAN (Elevator) algorithm** to handle requests. Instead of serving requests purely First-Come-First-Serve (which leads to inefficient zigzagging), the elevator continues traveling in its current direction, servicing all requests along the way until there are no more requests in that direction. 

### Class Structure (OOP Design)
*   `ElevatorSystem`: The main controller that receives external requests and delegates them to the most optimal elevator car.
*   `ElevatorCar`: Manages its own internal queue, current floor, direction, and capacity.
*   `Request`: A data model representing a passenger's source floor, destination floor, and direction.

## 🚀 Installation and Setup

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/madhav7802/vs-code-projects.git](https://github.com/madhav7802/vs-code-projects.git)
   cd vs-code-projects/elevator-simulation
