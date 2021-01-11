# Code Generation Tools

This folder contains Python tools to auto-generate C++ code.

The DXF format uses non uniform data types and some times different
solutions for different DXF versions. Because of the limited dynamic 
features of C++ and my lack of knowledge of clever template programming 
techniques, I decided to use my knowledge baked into the Python version 
of ezdxf to auto-generate C++ source code.

This tools are not integrated into the build process, all generated files
are included in final state in the repository.

- config.py: common configuration data

## Header Section Loader/Writer

- header_section_base_class.py: Header section base class with all 
  members and getter/setter methods
  
- header_section_loader.py: create for each DXF version a specialized 
  header section loader
  
- header_section_writer.py: create for each DXF version a specialized
  header section exporter