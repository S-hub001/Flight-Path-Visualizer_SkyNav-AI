# 🛩 ***Flight Path Visualizer - SkyNav AI***

🚀 **Flight Path Visualizer - SkyNav AI** is a **C++-based** application that allows users to **search, visualize, and book flight paths** between various destinations. The project features **graph-based flight management, visualization with SFML**, and support for **layovers and cost estimation**.

---

## 🌟 Features

### ✅ **Flight Management**
- Search for **direct and connecting flights** between cities.
- Book flights based on **cost, airline, and layover preferences**.
- Calculate **total travel cost**, including **hotel stay costs**.

### 🗺️ **Graph-Based Flight Path Visualization**
- Display **flight networks** using **SFML**.
- Highlight flights based on **cost (cheap, moderate, expensive)**.
- Show **airline-specific paths** and **layovers**.
- **Dijkstra’s Algorithm** for shortest path calculation.

### 🔎 **Advanced Flight Search**
- Search flights by **source, destination, and date**.
- Filter by **preferred airline** and **layover cities**.
- Identify **fastest and most cost-effective routes**.

---

## 🛠️ How It Works

### 1️⃣ **Graph Representation of Flight Network**
- Uses **adjacency lists** to store flight connections.
- **Nodes** represent cities, and **edges** represent flights.

### 2️⃣ **Flight Search & Booking**
- Users can **search for flights**, view **available routes**, and **book tickets**.
- The system **calculates layover feasibility** and **ensures sufficient transition time**.

### 3️⃣ **SFML-Based Visualization**
- Flight routes are **graphically displayed** with **real-time updates**.
- Supports **different color codes** for expensive, moderate, and budget-friendly flights.

---

## 📜 Code Overview

### **📌 Key Classes**
- **`flight`** → Stores flight details (airline, departure time, arrival time, cost).
- **`graph`** → Represents the flight network.
- **`JourneyNode`** → Manages **layover details**.
- **`Queue`** → Handles **flight priority and layover management**.

---

## ⚙️ Prerequisites

Ensure you have the following installed before running the application:

- **C++ Compiler** (**G++/MSVC**) with **C++11 or later**.
- **SFML (Simple and Fast Multimedia Library)** for visualization.
- **OpenGL** (for rendering graphics).

---


