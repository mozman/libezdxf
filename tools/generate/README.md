# Code Generation Tools

This folder contains Python tools to auto-generate C++ code. This tools 
are not integrated into the build process, all generated files are 
included in final state in the repository.

- config.py: common configuration data

To use this tools you need a working Python setup and the latest version of
ezdxf from PyPI:
```
pip install ezdxf
```

## Header Section Loader/Exporter

- header_section_base_class.py: Header section base class with all 
  members and getter/setter methods
  
- header_section_loader.py: create for each DXF version a specialized 
  header section loader
  
- header_section_exporter.py: create for each DXF version a specialized
  header section exporter