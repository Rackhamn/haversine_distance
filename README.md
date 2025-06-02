# Haversine Distance
> hav(theta) = (sin^2)(theta / 2)  

This program computes the haversine distance mostly accurate.  
It can compute the distance between Stockholm, New York and Antarctica (the South Pole).  

## Usage:
```c
latlon_t coord_a = { 59.3327, 18.0656 };
latlon_t coord_b = { 40.7128, 74.006  };
double d_km = hav(coord_a, coord_b);

/* or */

stockholm = { 59.3327, 18.0656 };
new_york  = { 40.7128, 74.006  };
d_km = hav(stockholm, new_york);

/* You can also convert to american and nautical miles */
double nautical_miles = KM2NM(d_km);
```

## Example Output:
```r
Stockholm   :  59.33270° N,  18.06560° E
New York    :  40.71280° N, -74.00600° W
Distance between Stockholm and New York is 6319.638 km

New York    :  40.71280° N,  74.00600° W
South Pole  : -90.00000° S,   0.00000° E
Distance between New York and South Pole is 14534.600 km
```
