#ifndef _MGA_UTILS_H
#define _MGA_UTILS_H

double gem_amp_power(gem gem1, gemO amp1, double leech_ratio)
{
	return gem1.bbound*(gem1.leech+leech_ratio*amp1.leech);
}

void print_omnia_table(gem* gems, gemO* amps, double* powers, int len)
{
	printf("Managem\tAmps\tPower\n");
	int i;
	for (i=0; i<len; i++)
		printf("%d\t%d\t%#.7g\n", i+1, gem_getvalue_O(amps+i), powers[i]);
	printf("\n");
}

/* Macro blobs used for compressions in various files. Handle with care */

#define MGSPEC_COMPRESSION\
	for (i=0;i<len;++i) { \
		int j; \
		gem* temp_pool=malloc(pool_length[i]*sizeof(gem)); \
		for (j=0; j<pool_length[i]; ++j) { \
			temp_pool[j]=pool[i][j]; \
		} \
		gem_sort_exact(temp_pool,pool_length[i]); \
		int broken=0; \
		float lim_bbound=-1; \
		for (j=pool_length[i]-1;j>=0;--j) { \
			if (temp_pool[j].bbound<=lim_bbound) { \
				temp_pool[j].grade=0; \
				broken++; \
			} \
			else lim_bbound=temp_pool[j].bbound; \
		} \
		gem best=(gem){0}; \
		for (j=0;j<pool_length[i];++j) \
		if (gem_more_powerful(temp_pool[j], best)) { \
			best=temp_pool[j]; \
		} \
		for (j=0;j<pool_length[i];++j) {\
			if (temp_pool[j].bbound < best.bbound \
			&&  temp_pool[j].grade!=0 \
			) { \
				temp_pool[j].grade=0;\
				broken++; \
			} \
		} \
		poolf_length[i]=pool_length[i]-broken; \
		poolf[i]=malloc(poolf_length[i]*sizeof(gem)); \
		int index=0; \
		for (j=0; j<pool_length[i]; ++j) { \
			if (temp_pool[j].grade!=0) { \
				poolf[i][index]=temp_pool[j]; \
				index++; \
			} \
		} \
		free(temp_pool); \
		if (output_options & mask_debug) printf("Managem value %d speccing compressed pool size:\t%d\n",i+1,poolf_length[i]); \
	}

#define MGCOMB_COMPRESSION\
	{ \
		gem_sort_exact(poolc[lenc-1],poolc_length[lenc-1]); \
		int broken=0; \
		float lim_bbound=-1; \
		for (i=poolc_length[lenc-1]-1;i>=0;--i) { \
			if (poolc[lenc-1][i].bbound<=lim_bbound) { \
				poolc[lenc-1][i].grade=0; \
				broken++; \
			} \
			else lim_bbound=poolc[lenc-1][i].bbound; \
		} \
		 \
		gem best=(gem){0}; \
		for (i=0;i<poolc_length[lenc-1];++i) \
		if (gem_more_powerful(poolc[lenc-1][i], best)) { \
			best=poolc[lenc-1][i]; \
		} \
		for (i=0;i<poolc_length[lenc-1];++i) { \
			if (poolc[lenc-1][i].bbound < best.bbound \
			&&  poolc[lenc-1][i].grade!=0 \
			) { \
				poolc[lenc-1][i].grade=0; \
				broken++; \
			} \
		} \
		poolcf_length=poolc_length[lenc-1]-broken; \
		poolcf=malloc(poolcf_length*sizeof(gem)); \
		int index=0; \
		for (i=0; i<poolc_length[lenc-1]; ++i) { \
			if (poolc[lenc-1][i].grade!=0) { \
				poolcf[index]=poolc[lenc-1][i]; \
				index++; \
			} \
		} \
	}

#define AMPS_COMPRESSION\
	for (i=0; i<lena; ++i) { \
		int j; \
		bestO[i]=(gemO){0}; \
		for (j=0; j<poolO_length[i]; ++j) { \
			if (gem_better(poolO[i][j], bestO[i])) { \
				bestO[i]=poolO[i][j]; \
			} \
		} \
	}


#endif // _MGA_UTILS_H