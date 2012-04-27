# Material Properties

Following the priciple of seperating data from logics, material properties are collected into .json files. Each material should have its own json file. The format should follow

```javascript
// for single value
"SCINTILLATIONYIELD": {"value": 90.0, "unit": "/MeV"}

// for array value
"RAYLEIGH": {"x": "ENERGY", "y": "WBLS_RAYLEIGH"}
```

For array values, the named array itself needs to be defined in the json file.

For all properties, you can add a 'scaling' parameter to indicate the normalizion factor to the default.

Note that the json file itself does not allow comments (which is a downside compared with, for example, yaml). It's supposed to be all 'data'.

The example python script in this directory shows how to decode the json file (which is super easy).
