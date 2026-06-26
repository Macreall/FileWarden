# FileWarden

*A lightweight Win32 desktop application that transforms basic scanner software into a configurable document management system.*

---

## Overview

FileWarden is a native Windows application written in **C using the Win32 API**. It monitors a scanner's output folder in real time and provides a customizable workflow for naming, organizing, and distributing scanned documents.

Designed for businesses that process hundreds or thousands of documents, FileWarden eliminates repetitive manual organization while keeping system resource usage extremely low.

---

## Features

* **Custom Naming Conventions**

  * Configure file names and folder structures to match your business workflow.

* **Unlimited Save Locations**

  * Save documents to any number of configured destinations instead of being limited by scanner software.

* **Multi-Destination Saving**

  * Automatically copy a scanned document to multiple folders simultaneously.

* **Configurable User Interface**

  * Customize save paths, naming conventions, UI components, and workflow behavior through configuration files.

* **PDF Processing**

  * Detects newly scanned PDF documents and presents an interface for organizing them before saving.

* **Background Operation**

  * Runs silently in the Windows system tray and automatically starts with Windows.

---

## The Problem

Many businesses rely on scanner software that offers limited organization capabilities.

As document volume increases, files accumulate in large folders, making retrieval slow, inconsistent, and time-consuming.

---

## The Solution

FileWarden continuously monitors a scanner's output directory.

Whenever a new document is scanned, it automatically launches a configurable workflow that allows users to:

* Rename documents
* Organize documents into custom directory structures
* Save to multiple locations
* Maintain a consistent filing system

The result is a significantly faster and more organized document management process.

---

## Technical Details

**Language**

* C

**Framework**

* Win32 API

**Configuration**

* INI Files

**Document Handling**

* PDF Processing

**Architecture**

* Event-driven Windows application
* File system monitoring
* Windows system tray integration

---

## Performance

* Executable Size: **~140 KB**
* Runtime Memory Usage: **10–20 MB**
* Designed to run continuously (24/7)
* Lightweight native Win32 implementation

---

## Installation

1. Create the folder:

```text
C:\watchFolder
```

2. Place `settings.ini` inside the watch folder.

3. Configure your scanner to save scanned PDFs into:

```text
C:\watchFolder
```

4. Launch FileWarden.

When a new PDF is detected, FileWarden automatically opens its document processing window, allowing you to customize file names and save locations before organizing the document.

---

## Ideal Use Cases

FileWarden is designed for organizations that regularly process large volumes of paperwork, including:

* Automotive repair shops
* Medical offices
* Law firms
* Accounting firms
* Government offices
* Any business managing large collections of scanned documents

---

## Future Development

* OCR-assisted document classification
* Cloud storage integration
* Advanced search capabilities
* Custom workflow templates
* Multi-user configuration support
