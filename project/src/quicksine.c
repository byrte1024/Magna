//quicksine.c

#include "quicksine.h" 

double qsine_segmant_db[QSINE_SAMPLES];
int qsine_segmant_int[QSINE_SAMPLES];

void m_quicksine_init() {
	for (int i = 0; i < QSINE_SAMPLES; i++) {
		//Generate a quarter of a sine wave
		double angle = (double)i * (HALF_PI / (QSINE_SAMPLES - 1));
		qsine_segmant_db[i] = sin(angle);

	}
	printf("Quicksine initialized with %d samples.\n", QSINE_SAMPLES);

	int matches = 0;
	int mismatches = 0;

	double avg_diff = 0.0;

	//Make sure quicksine works as intended
	for (int i = 0; i < 1000; i++) {
		double angle = (double)i * (TWO_PI / 1000.0);

		double ourvalue = m_sin(angle);
		double refvalue = sin(angle);

		double diff = fabs(ourvalue - refvalue);

		//add to avg diff
		avg_diff += diff;

		if (diff < 0.0001) {
			matches++;
		}
		else {
			mismatches++;
		}		
	}

	avg_diff /= 1000.0;
	printf("Quicksine matches: %d, mismatches: %d, avg diff: %f\n", matches, mismatches, avg_diff);
	if (mismatches > 0) {
		fprintf(stderr, "Warning: Quicksine has mismatches with the standard sin function. stuff may go wrong!\n");
	}
	else {
		printf("Quicksine is fully compatible with the standard sin function.\n");
	}
}

