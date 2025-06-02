#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> // -lm

// * https://en.wikipedia.org/wiki/Geographic_coordinate_system
// * https://cs.nyu.edu/~visual/home/proj/tiger/gisfaq.html

#define PI		3.1415926535897932384626434
#define TAU		6.2831853071795864769252868
#define EARTH_RADIUS_KM	6367 // 6371.0

// angle (degrees) to radian
#define A2R(a)		((a) * (PI / 180.0))

// kilometers to nautical miles
#define KM2NM(x)	((x) / 1.852)
// kilometers to American miles
#define KM2AM(X)	((x) / 1.609)

#define NORTH_CHAR	'N'
#define SOUTH_CHAR	'S'
#define WEST_CHAR	'W'
#define EAST_CHAR	'E'

#define DEGREE_STR	"°"

struct dms_s {
	double degrees, minutes, seconds;
};
typedef struct dms_s dms_t;

#define DMS2DD(x) ((x).degrees + ((x).minutes / 60.0) + ((x).seconds / 3600.0))
double dms_to_dd(double degree, double minute, double second, char dir) {
	double dd = degree + (minute / 60.0) + (second / 3600.0);
	if(dir == SOUTH_CHAR || dir == WEST_CHAR) {
		return -dd;
	}
	return dd;
}

struct latlon_s {
	union {
		double deg[2];
		struct { double lat, lon; };
	};
	union {
		char dir[2];
		struct { char lat_dir, lon_dir; };
	};
};
typedef struct latlon_s latlon_t;

struct location_s {
	char * name;
	latlon_t coord;
};
typedef struct location_s location_t;

latlon_t norm_coord(latlon_t x) {
	x.lat = (x.lat_dir == SOUTH_CHAR) ? -fabs(x.lat) : fabs(x.lat);
	x.lon = (x.lon_dir == WEST_CHAR) ? -fabs(x.lon) : fabs(x.lon);

	return x;
}

void printf_latlon(latlon_t coord) {
	printf("%9.5f%s %c, %9.5f%s %c\n", 
		coord.lat, DEGREE_STR, coord.lat_dir,
		coord.lon, DEGREE_STR, coord.lon_dir);
}

void printf_location(location_t loc) {
	printf("%-12s: ", loc.name);
	printf_latlon(loc.coord);
}

location_t make_location(char * name, double lat, double lon) {
	char NS = (lat < 0) ? SOUTH_CHAR : NORTH_CHAR;
	char EW = (lon < 0) ? WEST_CHAR : EAST_CHAR;

	/*
	lat = fabs(lat);
	lon = fabs(lon);
	*/

	location_t loc = { name, { lat, lon, NS, EW } };

	return loc;
}

location_t make_location_dms(char * name, dms_t dms_lat, dms_t dms_long) {
	double lat = DMS2DD(dms_lat);
	double lon = DMS2DD(dms_lon);

	return make_location(name, lat, lon);
}

// hav(theta) == (sin^2)(theta / 2)
//
// hav() =
// 	d_phi = radians(lat2 - lat1)
// 	d_lambda = radians(lon2 - lon1)
//
// 	a = (sin^2)(d_phi / 2.0) + 
// 		cos(radians(lat1)) * cos(radians(lat2)) * 
// 		(sin^2)(d_lambda / 2.0)
//
// 	c = 2 * atan2(sqrt(a), sqrt(1.0 - a))
//
// 	R := "earth radius in km"
// 	d = R * c
//
double hav(latlon_t l1, latlon_t l2) {
	// * https://cs.nyu.edu/~visual/home/proj/tiger/gisfaq.html
	// d = 2 asin(min(1, sqrt(sin(dl/2)^2 + cos(l1)cos(l2)sin(dL/2)^2)))

	double _lat = A2R(l2.lat - l1.lat);
	double _lon = A2R(l2.lon - l1.lon);

	double lat1 = A2R(l1.lat);
	double lat2 = A2R(l2.lat);

	double sin2_lat = sin(_lat / 2.0);
	double sin2_lon = sin(_lon / 2.0);

	double a = (sin2_lat * sin2_lat) +
			cos(lat1) * cos(lat2) *
			(sin2_lon * sin2_lon);

	double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

	return (EARTH_RADIUS_KM * c);
}

double loc_dist(location_t a, location_t b) {
	a.coord = norm_coord(a.coord);
	b.coord = norm_coord(b.coord);

	double d = hav(a.coord, b.coord);
	return d;
}

void print_location_dist(location_t a, location_t b) {
	double d_km = loc_dist(a, b);

	printf_location(a);
	printf_location(b);
	printf("Distance between %s and %s is %.3f km\n\n", 
			a.name, b.name, 
			d_km);
}

int main(void) {
	location_t locations[] = {
		{ "Stockholm",  {  59.3327,  18.0656, NORTH_CHAR, EAST_CHAR } },
		{ "New York",   {  40.7128, -73.9352, NORTH_CHAR, WEST_CHAR } },
		{ "Antarctica", {  82.8628,  135.000, SOUTH_CHAR, EAST_CHAR } },
		{ "South Pole", { -90.0000,  0.00000, SOUTH_CHAR, EAST_CHAR } },
	};
	int num_locations = sizeof(locations)/sizeof(locations[0]);

	locations[3] = make_location("Paris", 45.7597, 4.8422);

#if 1
	for(int i = 0; i < num_locations; i++) {
		for(int j = 0; j < num_locations; j++) {
			if(i != j)
				print_location_dist(locations[i], locations[j]);
		}
	}
#else
	print_location_dist(locations[0], locations[1]);
	print_location_dist(locations[1], locations[3]);
#endif

	return 0;
};
