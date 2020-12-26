# Data Types and Units

## Handles

Handles are stored as `uint64_t`. The handles are used as keys for the entity 
database. The handle `0` is a special case and not a valid database key.


## Angles

Angles will be stored as radians the native angle unit for computers.
The DXF format store most angles as decimal degrees and some angles as radians,
the C++ implementation of `ezdxf` will unify this confusing behavior by always 
using radians (AutoLisp also uses only radians).

## Colors

### ACI

The Autocad Color Index is stored as an unsigned integer.

### RGB Channels

True color values are stored as floats in the range from `0.0` to `1.0` for each 
color channel.

### Alpha Channel 

The alpha channel is represented by a float following the [CSS definition][1] 
where `0.0` is fully transparent and `1.0` is fully opaque.

## Resources

All resources like layers, text styles, dimension styles, block references and 
so on will be stored as handles to the table entries like in the DWG format. 
This requires a resolving process because the DXF format stores many resources 
by name as case insensitive strings. DXF allows some resources without existing 
table entries (e.g. layers), which has to be fixed by automatically creating 
default table entries for such resources.

### Resource Tables

- Text style (STYLE): no resource references
- Shape file (STYLE): no resource references. This is technically a text style 
  but without a name -- I will not replicate this crappy design and therefore 
  shape files get a separated resource table
- Line type (LTYPE): may contain resource references by name
- Layer (LAYER): references line types by name, other resources are referenced 
  by handles
- Dimension style: R12 references resources by name, later DXF formats references 
  resources by handles.
- Application identifier (APPID): no resource references
- UCS: no resource references
- Views (VIEW): resources referenced by handles
- Viewports (VPORT): some resources are referenced by name. A viewport 
  configuration consist of one or more VPORT entries.  
- Block record (BLOCK_RECORD): associated BLOCK is referenced by name, LAYOUT is
  referenced by handle

### Resource Objects

Objects were introduced in the DXF R13+ format and they use always (?) handles 
to reference resources.

- Material
- Plot style
- Layout definition
- MLine style
- MLeader style
- Visual style
- Table style
- ImageDef
- UnderlayDef

[1]: https://developer.mozilla.org/en-US/docs/Web/CSS/alpha-value