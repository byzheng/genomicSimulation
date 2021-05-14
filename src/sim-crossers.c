#include "sim-crossers.h"

GenOptions create_genoptions(SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions go = BASIC_OPT;
	int b;
	
	b = asLogical(name);
	if (b == NA_LOGICAL) { error("`name` parameter is of invalid type.\n"); }
	go.will_name_subjects = b;
	
	go.subject_prefix = CHAR(asChar(namePrefix));
	
	b = asInteger(familySize);
	if (b == NA_INTEGER) { error("`times` parameter is of invalid type.\n"); }
	go.family_size = b;
	
	b = asLogical(trackPedigree);
	if (b == NA_LOGICAL) { error("`track.pedigree` parameter is of invalid type.\n"); }
	go.will_track_pedigree = b;
	
	b = asLogical(giveIds);
	if (b == NA_LOGICAL) { error("`give.ids` parameter is of invalid type.\n"); }
	go.will_allocate_ids = b;
	
	go.filename_prefix = CHAR(asChar(filePrefix));
	
	b = asLogical(savePedigree);
	if (b == NA_LOGICAL) { error("`save.pedigree` parameter is of invalid type.\n"); }
	go.will_save_pedigree_to_file = b;
	b = asLogical(saveEffects);
	if (b == NA_LOGICAL) { error("`save.gebv` parameter is of invalid type.\n"); }
	go.will_save_effects_to_file = b;
	b = asLogical(saveGenes);
	if (b == NA_LOGICAL) { error("`save.genotype` parameter is of invalid type.\n"); }
	go.will_save_genes_to_file = b;
	
	b = asLogical(retain);
	if (b == NA_LOGICAL) { error("`retain` parameter is of invalid type.\n"); }
	go.will_save_to_simdata = b;
	
	return go;
}

SEXP SXP_cross_randomly(SEXP exd, SEXP glen, SEXP groups, SEXP crosses, SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);

	int len = asInteger(glen);
	int *gps = INTEGER(groups); 
	if (len == NA_INTEGER) { 
		error("`groups` is invalid.\n");
	}
	for (int i = 0; i < len; ++i) {
		if (gps[i] == NA_INTEGER || gps[i] < 0) { error("The contents of `groups` is invalid.\n"); }
	}
	
	int n = asInteger(crosses);
	if (n < 0 || n == NA_INTEGER) { error("`n.crosses` parameter is invalid.\n"); }
	
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);

	if (len == 1) {
		return ScalarInteger(cross_random_individuals(d, gps[0], n, g));
		
	} else {
		// Get an R vector of the same length as the number of new size 1 groups created
		SEXP out = PROTECT(allocVector(INTSXP, len));
		int* outc = INTEGER(out);
		for (int i = 0; i < len; ++i) {
			outc[i] = cross_random_individuals(d, gps[i], n, g);
		}
		UNPROTECT(1);
		return out;
	}
}

SEXP SXP_cross_combinations(SEXP exd, SEXP filename, SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);
	
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);
	
	const char* fname = CHAR(asChar(filename));
	
	return ScalarInteger(make_crosses_from_file(d, fname, g));
}

SEXP SXP_dcross_combinations(SEXP exd, SEXP filename, SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);
									 
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);
	
	const char* fname = CHAR(asChar(filename));
	
	return ScalarInteger(make_double_crosses_from_file(d, fname, g));
	
}

SEXP SXP_cross_unidirectional(SEXP exd, SEXP glen, SEXP groups, SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);
	int len = asInteger(glen);
	int *gps = INTEGER(groups); 
	if (len == NA_INTEGER) { 
		error("`groups` vector is invalid.\n");
	}
	for (int i = 0; i < len; ++i) {
		if (gps[i] == NA_INTEGER || gps[i] < 0) { error("The contents of `groups` is invalid.\n"); }
	}
	
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);
	
	if (len == 1) {
		return ScalarInteger(make_all_unidirectional_crosses(d, gps[0], g));
	} else {
		// Get an R vector of the same length as the number of new size 1 groups created
		SEXP out = PROTECT(allocVector(INTSXP, len));
		int* outc = INTEGER(out);
		for (int i = 0; i < len; ++i) {
			outc[i] = make_all_unidirectional_crosses(d, gps[i], g);
		}
		UNPROTECT(1);
		return out;
	}
}

/*SEXP cross_top(SEXP exd, SEXP group, SEXP percent, SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);
	int grp = asInteger(group);
	if (grp < 0 || grp == NA_INTEGER) { error("`group` parameter is invalid.\n"); }
	
	double cpercent = asReal(percent);
	if (cpercent < 0 || cpercent > 100) { error("`threshold` parameter is invalid.\n"); }
	
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);
	if (d->e.effects.matrix == NULL) { error("Need to load effect values before running this function.\n"); } 
	
	return ScalarInteger(make_crosses_from_top_m_percent(d, cpercent, grp, g)); 
}*/

SEXP SXP_selfing(SEXP exd, SEXP glen, SEXP groups, SEXP n, SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);
	int len = asInteger(glen);
	int *gps = INTEGER(groups); 
	if (len == NA_INTEGER) { 
		error("`groups` vector is invalid.\n");
	}
	for (int i = 0; i < len; ++i) {
		if (gps[i] == NA_INTEGER || gps[i] < 0) { error("The contents of `groups` is invalid.\n"); }
	}
	
	int cn = asInteger(n);
	if (cn < 0 || cn == NA_INTEGER) { error("`n` parameter is invalid.\n"); }
	
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);
	
	if (len == 1) {
		return ScalarInteger(self_n_times(d, cn, gps[0], g));
	} else {
		// Get an R vector of the same length as the number of new size 1 groups created
		SEXP out = PROTECT(allocVector(INTSXP, len));
		int* outc = INTEGER(out);
		for (int i = 0; i < len; ++i) {
			outc[i] = self_n_times(d, cn, gps[i], g);
		}
		UNPROTECT(1);
		return out;
	}
}

SEXP SXP_doubled(SEXP exd, SEXP glen, SEXP groups, SEXP name, SEXP namePrefix, SEXP familySize,
		SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, SEXP savePedigree,
		SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);
	int len = asInteger(glen);
	int *gps = INTEGER(groups); 
	if (len == NA_INTEGER) { 
		error("`groups` vector is invalid.\n");
	}
	for (int i = 0; i < len; ++i) {
		if (gps[i] == NA_INTEGER || gps[i] < 0) { error("The contents of `groups` is invalid.\n"); }
	}
	
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);
	
	if (len == 1) {
		return ScalarInteger(make_doubled_haploids(d, gps[0], g));
	} else {
		// Get an R vector of the same length as the number of new size 1 groups created
		SEXP out = PROTECT(allocVector(INTSXP, len));
		int* outc = INTEGER(out);
		for (int i = 0; i < len; ++i) {
			outc[i] = make_doubled_haploids(d, gps[i], g);
		}
		UNPROTECT(1);
		return out;
	}
}

SEXP SXP_one_cross(SEXP exd, SEXP parent1_index, SEXP parent2_index, SEXP name, 
		SEXP namePrefix, SEXP familySize, SEXP trackPedigree, SEXP giveIds, SEXP filePrefix, 
		SEXP savePedigree, SEXP saveEffects, SEXP saveGenes, SEXP retain) {
	GenOptions g = create_genoptions(name, namePrefix, familySize, trackPedigree,
									 giveIds, filePrefix, savePedigree, saveEffects,
									 saveGenes, retain);
									 
	int p1 = asInteger(parent1_index);
	if (p1 == NA_INTEGER) { error("`parent1.index` parameter is of invalid type.\n"); }
	
	int p2 = asInteger(parent2_index);
	if (p2 == NA_INTEGER) { error("`parent2.index` parameter is of invalid type.\n"); }
	
	SimData* d = (SimData*) R_ExternalPtrAddr(exd);
	
	int instructions[2][1];
	instructions[0][0] = p1;
	instructions[1][0] = p2;
	
	return ScalarInteger(cross_these_combinations(d, 1, instructions, g));
}


/*--------------------------------Crossing-----------------------------------*/

/** Fills a char* with the simulated result of meiosis (reduction and
 * recombination) from the marker alleles of a given parent.
 *
 * It generates the number of crossover events in each chromosome by drawing
 * from a Poisson distribution with parameter corresponding to the length of
 * the chromosome (in Morgans.). 
 *
 * It generates the positions of those crossover events from a uniform distribution.
 *
 * It picks a random one of the gametes generated by picking randomly which column
 * of the parent's alleles to start with. When crossover events occur it starts reading
 * the other column.
 *
 * @param d pointer to the SimData object containing map positions for the markers
 * that make up the rows of `parent_table`. sort_markers() and locate_chromosomes()
 * should have been called previously.
 * @param parent_genome the char* containing the parent's genome as a character string
 * made up of sequential pairs of alleles for each marker in d->markers.
 * @param output the char* to which to save the gamete. It saves the alleles every second
 * character, starting at 0, so that calling generate_gamete(..., offspring_genome) & 
 * generate_gamete(..., offspring_genome + 1) can be used to generate both halves of its genome.
*/
void generate_gamete(SimData* d, char* parent_genome, char* output) {
	// assumes rand is already seeded
	if (parent_genome == NULL) {
		warning("Could not generate this gamete\n");
		return;
	}
	
	int num_crossovers, up_to_crossover, which;
	float crossover_where[100];
	float* p_crossover_where;
	
	// treat each chromosome separately.
	for (int chr = 1; chr <= d->map.n_chr; ++chr) {
		// use Poisson distribution to choose the number of crossovers in this chromosome
		num_crossovers = Rf_rpois(d->map.chr_lengths[chr - 1] / 100);
		
		// in the rare case where it could be >100, get enough space
		// to be able to store the crossover positions we're about to create
		if (num_crossovers <= 100) {
			p_crossover_where = crossover_where; // point at the start of array
		} else {
			p_crossover_where = get_malloc(sizeof(float) * num_crossovers);
		}
		
		// TASK 3: choose points where those crossovers occur
		// by randomly generating a point along the length of the chromosome
		for (int i = 0; i < num_crossovers; ++i) {
			p_crossover_where[i] = ((double)rand() / (double)RAND_MAX) 
				* d->map.chr_lengths[chr - 1] 
				+ d->map.positions[d->map.chr_ends[chr - 1]].position;
		}
		
		// sort the crossover points
		if (num_crossovers > 1) {
			qsort(p_crossover_where, num_crossovers, sizeof(float),
					_ascending_float_comparer);
		}
		
		// pick a parent genome half at random
		which = (unif_rand() > 0.5); // if this is 0, we start with the left.
		
		// TASK 4: Figure out the gamete that those numbers produce.
		up_to_crossover = 0; // which crossovers we've dealt with
		for (int i = d->map.chr_ends[chr - 1]; i < d->map.chr_ends[chr]; ++i) {
			// loop through every marker for this chromosome
			if (up_to_crossover < num_crossovers && 
					d->map.positions[i].position > p_crossover_where[up_to_crossover]) {
				// if we're here then between last loop and this one we crossed over.
				// invert which and update up_to_crossover;
				which = 1 - which;
				up_to_crossover += 1;
			}
			output[2*i] = parent_genome[2*i + which];
		}
		
		if (num_crossovers > 100) {
			free(p_crossover_where);
		}
		
	}
	return;
}

/** Get the alleles of the outcome of crossing two subjects 
 *
 * Gametes are generated at the same time but are independent.
 *
 * @see generate_gamete(), on which this is based.
 * 
 * @param d pointer to the SimData object that includes genetic map data 
 * needed to simulate meiosis and the value of n_markers
 * @param parent1_genome a 2x(n_markers) array of characters containing the 
 * alleles of the first parent
 * @param parent2_genome a 2x(n_markers) array of characters containing the 
 * alleles of the second parent.
 * @param output a 2x(n_marker) array of chars which will be overwritten
 * with the offspring genome.
*/
void generate_cross(SimData* d, char* parent1_genome, char* parent2_genome, char* output) {
	// assumes rand is already seeded
	if (parent1_genome == NULL || parent2_genome == NULL) {
		warning("Could not generate this cross\n");
		return;
	}
	
	int num_crossovers[2], up_to_crossover[2], which[2];
	float* p_crossover_where[2];
	float crossover_where[2][50];
	
	// treat each chromosome separately.
	for (int chr = 1; chr <= d->map.n_chr; ++chr) {
		// use Poisson distribution to choose the number of crossovers in this chromosome
		num_crossovers[0] = Rf_rpois(d->map.chr_lengths[chr - 1] / 100);
		num_crossovers[1] = Rf_rpois(d->map.chr_lengths[chr - 1] / 100);
		
		// in the rare case where it could be >100, get enough space
		// to be able to store the crossover positions we're about to create
		if (num_crossovers[0] <= 50) {
			p_crossover_where[0] = crossover_where[0]; // point to start of array
		} else {
			p_crossover_where[0] = get_malloc(sizeof(float) * num_crossovers[0]);
		}
		if (num_crossovers[1] <= 50) { 
			p_crossover_where[1] = crossover_where[0]; // point to start of array
		} else {
			p_crossover_where[1] = get_malloc(sizeof(float) * num_crossovers[1]);
		}
		
		// TASK 3: choose points where those crossovers occur
		// by randomly generating a point along the length of the chromosome
		for (int i = 0; i < num_crossovers[0]; ++i) {
			p_crossover_where[0][i] = ((double)rand() / (double)RAND_MAX) 
				* d->map.chr_lengths[chr - 1] 
				+ d->map.positions[d->map.chr_ends[chr - 1]].position;
		}
		for (int i = 0; i < num_crossovers[1]; ++i) {
			p_crossover_where[1][i] = ((double)rand() / (double)RAND_MAX) 
				* d->map.chr_lengths[chr - 1] 
				+ d->map.positions[d->map.chr_ends[chr - 1]].position;
		}
		
		// sort the crossover points
		if (num_crossovers[0] > 1) {
			qsort(p_crossover_where[0], num_crossovers[0], sizeof(float),
					_ascending_float_comparer);
		}
		if (num_crossovers[1] > 1) {
			qsort(p_crossover_where[1], num_crossovers[1], sizeof(float),
					_ascending_float_comparer);
		}
		
		// pick a parent genome half at random
		which[0] = (unif_rand() > 0.5); which[1] = (unif_rand() > 0.5); // if this is 0, we start with the left.
		
		// TASK 4: Figure out the gamete that those numbers produce.
		up_to_crossover[0] = 0; up_to_crossover[1] = 0; // which crossovers we've dealt with
		for (int i = d->map.chr_ends[chr - 1]; i < d->map.chr_ends[chr]; ++i) {
			// loop through every marker for this chromosome
			if (up_to_crossover[0] < num_crossovers[0] && 
					d->map.positions[i].position > p_crossover_where[0][up_to_crossover[0]]) {
				// between last loop and this one we crossed over.
				// invert which and update up_to_crossover;
				which[0] = 1 - which[0];
				up_to_crossover[0] += 1;
			}
			if (up_to_crossover[1] < num_crossovers[1] && 
					d->map.positions[i].position > p_crossover_where[1][up_to_crossover[1]]) {
				which[1] = 1 - which[1];
				up_to_crossover[1] += 1;
			}
			output[2*i] = parent1_genome[2*i + which[0]];
			output[2*i + 1] = parent2_genome[2*i + which[1]];
		}
		
		if (num_crossovers[0] > 50) {
			free(p_crossover_where[0]);
		}
		if (num_crossovers[1] > 50) {
			free(p_crossover_where[1]);
		}
		
	}
	return;
}

/** Get the alleles of the outcome of producing a doubled haploid from 
 * a gamete from a given parent.
 *
 * One gamete is generated, then doubled. The output will be perfectly 
 * homozygous.
 *
 * @see generate_gamete(), on which this is based.
 * 
 * @param d pointer to the SimData object that includes genetic map data 
 * needed to simulate meiosis and the value of n_markers
 * @param parent_genome a 2x(n_markers) array of characters containing the 
 * alleles of the first parent
 * @param output a 2x(n_marker) array of chars which will be overwritten
 * with the offspring genome.
*/
void generate_doubled_haploid(SimData* d, char* parent_genome, char* output) {
	// assumes rand is already seeded
	if (parent_genome == NULL) {
		warning("Could not make this doubled haploid\n");
		return;
	}
	
	int num_crossovers, up_to_crossover, which;
	float crossover_where[100];
	float* p_crossover_where;
	
	// treat each chromosome separately.
	for (int chr = 1; chr <= d->map.n_chr; ++chr) {
		// use Poisson distribution to choose the number of crossovers in this chromosome
		num_crossovers = Rf_rpois(d->map.chr_lengths[chr - 1] / 100);
		
		// in the rare case where it could be >100, get enough space
		// to be able to store the crossover positions we're about to create
		if (num_crossovers <= 100) {
			p_crossover_where = crossover_where; // point at the start of array
		} else {
			p_crossover_where = get_malloc(sizeof(float) * num_crossovers);
		}
		
		// TASK 3: choose points where those crossovers occur
		// by randomly generating a point along the length of the chromosome
		for (int i = 0; i < num_crossovers; ++i) {
			p_crossover_where[i] = ((double)rand() / (double)RAND_MAX) 
				* d->map.chr_lengths[chr - 1] 
				+ d->map.positions[d->map.chr_ends[chr - 1]].position;
		}
		
		// sort the crossover points
		if (num_crossovers > 1) {
			qsort(p_crossover_where, num_crossovers, sizeof(float),
					_ascending_float_comparer);
		}
		
		// pick a parent genome half at random
		which = (unif_rand() > 0.5); // if this is 0, we start with the left.
		
		// TASK 4: Figure out the gamete that those numbers produce.
		up_to_crossover = 0; // which crossovers we've dealt with
		for (int i = d->map.chr_ends[chr - 1]; i < d->map.chr_ends[chr]; ++i) {
			// loop through every marker for this chromosome
			if (up_to_crossover < num_crossovers && 
					d->map.positions[i].position > p_crossover_where[up_to_crossover]) {
				// if we're here then between last loop and this one we crossed over.
				// invert which and update up_to_crossover;
				which = 1 - which;
				up_to_crossover += 1;
			}
			output[2*i] = parent_genome[2*i + which];
			output[2*i + 1] = parent_genome[2*i + which];
		}
		
		if (num_crossovers > 100) {
			free(p_crossover_where);
		}
		
	}
	return;
}

/** Performs random crosses among members of a group. If the group does not 
 * have at least two members, the simulation exits. Selfing/crossing an individual
 * with itself is not permitted. The resulting genotypes are allocated to a new group.
 *
 * Preferences in GenOptions are applied to this cross. The family_size parameter
 * in GenOptions allows you to repeat each particular randomly-chosen cross a 
 * certain number of times.
 *
 * Parents are drawn uniformly from the group when picking which crosses to make.
 *
 * @param d pointer to the SimData object that contains the genetic map and 
 * genotypes of the parent group.
 * @param from_group group number from which to draw the parents.
 * @param n_crosses number of random pairs of parents to cross.
 * @param g options for the genotypes created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
*/
int cross_random_individuals(SimData* d, int from_group, int n_crosses, GenOptions g) {
	int g_size = get_group_size( d, from_group);
	if (g_size < 2) {
		if (g_size == 1) {
			error("Group must contain multiple individuals to be able to perform random crossing\n");
		} else {
			error("Group %d does not exist.\n", from_group);
		}
	}
	char** group_genes = get_group_genes( d, from_group, g_size);
	
	// create the buffer we'll use to save the output crosses before they're printed.
	AlleleMatrix* crosses;
	int n_combinations = n_crosses * g.family_size;
	int n_to_go = n_combinations;
	if (n_to_go < 1000) {
		crosses = create_empty_allelematrix(d->n_markers, n_to_go);
		n_to_go = 0;
	} else {
		crosses = create_empty_allelematrix(d->n_markers, 1000);
		n_to_go -= 1000;
	}
	int fullness = 0;
	int parent1;
	int parent2;
	
	// set up pedigree/id allocation, if applicable
	unsigned int cid = 0;
	unsigned int* cross_current_id;
	if (g.will_allocate_ids) {
		cross_current_id = &(d->current_id);
	} else {
		cross_current_id = &cid;
	}
	unsigned int* group_ids = NULL;
	if (g.will_track_pedigree) {
		group_ids = get_group_ids( d, from_group, g_size);
	}
	AlleleMatrix* last = NULL;
	int output_group = 0;
	if (g.will_save_to_simdata) {
		last = d->m; // for saving to simdata
		while (last->next != NULL) {
			last = last->next;
		}
		output_group = get_new_group_num( d);
	}
	
	// open the output files, if applicable
	char fname[100];
	FILE* fp = NULL, * fe = NULL, * fg = NULL;
	DecimalMatrix eff;
	if (g.will_save_pedigree_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-pedigree");
		fp = fopen(fname, "w");
	}
	if (g.will_save_effects_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-eff");
		fe = fopen(fname, "w");
	}
	if (g.will_save_genes_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-genome");
		fg = fopen(fname, "w");
	}
	
	GetRNGstate();
	// loop through each combination
	for (int i = 0; i < n_crosses; ++i) {
		R_CheckUserInterrupt();
		
		// get parents, randomly.
		parent1 = round(unif_rand() * (g_size - 1));
		do {
			parent2 = round(unif_rand() * (g_size - 1));
		} while (parent1 == parent2);
		
		// do the cross.
		for (int f = 0; f < g.family_size; ++f, ++fullness) {
			// when cross buffer is full, save these outcomes to the file.
			if (fullness >= 1000) {
				crosses->n_subjects = 1000;
				// give the subjects their ids and names
				if (g.will_name_subjects) {
					set_subject_names(crosses, g.subject_prefix, *cross_current_id, 0);
				}
				for (int j = 0; j < 1000; ++j) {
					++ *cross_current_id;
					crosses->ids[j] = *cross_current_id;
				}
				
				// save the subjects to files if appropriate
				if (g.will_save_pedigree_to_file) {
					save_AM_pedigree( fp, crosses, d);
				}
				if (g.will_save_effects_to_file) {
					eff = calculate_fitness_metric( crosses, &(d->e));
					save_fitness( fe, &eff, crosses->ids, crosses->subject_names);
					delete_dmatrix( &eff);
				}
				if (g.will_save_genes_to_file) {
					save_allele_matrix( fg, crosses, d->markers);
				}
				
				if (g.will_save_to_simdata) {	
					last->next = crosses;
					last = last->next;
					if (n_to_go < 1000) {
						crosses = create_empty_allelematrix(d->n_markers, n_to_go);
						n_to_go = 0;
					} else {
						crosses = create_empty_allelematrix(d->n_markers, 1000);
						n_to_go -= 1000;
					}
				}
				
				fullness = 0; //reset the count and start refilling the matrix
			}
			
			generate_cross( d, group_genes[parent1] , group_genes[parent2] , crosses->alleles[fullness] );
			crosses->groups[fullness] = output_group;
			if (g.will_track_pedigree) {
				crosses->pedigrees[0][fullness] = group_ids[parent1];
				crosses->pedigrees[1][fullness] = group_ids[parent2];
			}
		}
		
	}
	PutRNGstate();
	
	// save the rest of the crosses to the file.
	free(group_genes);
	// give the subjects their ids and names
	if (g.will_name_subjects) {
		set_subject_names(crosses, g.subject_prefix, *cross_current_id, 0);
	}
	for (int j = 0; j < fullness; ++j) {
		++ *cross_current_id;
		crosses->ids[j] = *cross_current_id;
	}
	if (g.will_track_pedigree) {
		free(group_ids);
	}
	
	// save the subjects to files if appropriate
	if (g.will_save_pedigree_to_file) {
		save_AM_pedigree( fp, crosses, d);
		fclose(fp);
	}
	if (g.will_save_effects_to_file) {
		eff = calculate_fitness_metric( crosses, &(d->e));
		save_fitness( fe, &eff, crosses->ids, crosses->subject_names);
		delete_dmatrix( &eff);
		fclose(fe);
	}
	if (g.will_save_genes_to_file) {
		save_allele_matrix( fg, crosses, d->markers);
		fclose(fg);
	}
	if (g.will_save_to_simdata) {
		last->next = crosses;
		condense_allele_matrix( d );
		return output_group;
		
	} else {
		delete_allele_matrix( crosses );
		return 0;
	}
}

/** Performs the crosses of pairs of parents whose ids are provided in an array. The 
 * resulting genotypes are allocated to a new group.
 *
 * Preferences in GenOptions are applied to this cross. The family_size parameter
 * in GenOptions allows you to repeat each particular cross a 
 * certain number of times.
 *
 * @param d pointer to the SimData object that includes genetic map data and
 * allele data needed to simulate crossing.
 * @param combinations a 2D array of IDs, with the first dimension being parent 1 or 2,
 * and the second being the IDs of those parents for each combination to cross.
 * @param n_combinations the number of pairs of ids to cross/the length of `combinations`
 * @param g options for the genotypes created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
 */
int cross_these_combinations(SimData* d, int n_combinations, int combinations[2][n_combinations],  GenOptions g) {	
	if (n_combinations < 1) {
		return 0;
	}

	// create the buffer we'll use to save the output crosses before they're printed.
	AlleleMatrix* crosses;
	int n_to_go = n_combinations * g.family_size;
	if (n_to_go < 1000) {
		crosses = create_empty_allelematrix(d->n_markers, n_to_go);
		n_to_go = 0;
	} else {
		crosses = create_empty_allelematrix(d->n_markers, 1000);
		n_to_go -= 1000;
	}
	int fullness = 0, parent1id, parent2id;
	char* parent1genes, * parent2genes;
	
	// set up pedigree/id allocation, if applicable
	unsigned int cid = 0;
	unsigned int* cross_current_id;
	if (g.will_allocate_ids) {
		cross_current_id = &(d->current_id);
	} else {
		cross_current_id = &cid;
	}
	AlleleMatrix* last = NULL;
	int output_group = 0;
	if (g.will_save_to_simdata) {
		last = d->m; // for saving to simdata
		while (last->next != NULL) {
			last = last->next;
		}
		output_group = get_new_group_num( d);
	}
	
	// open the output files, if applicable
	char fname[100];
	FILE* fp = NULL, * fe = NULL, * fg = NULL;
	DecimalMatrix eff;
	if (g.will_save_pedigree_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-pedigree");
		fp = fopen(fname, "w");
	}
	if (g.will_save_effects_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-eff");
		fe = fopen(fname, "w");
	}
	if (g.will_save_genes_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-genome");
		fg = fopen(fname, "w");
	}
	
	GetRNGstate();
	// loop through each combination
	for (int i = 0; i < n_combinations; ++i) {
		R_CheckUserInterrupt();
		// find the parents & do the cross. First ensure parents are valid
		if (combinations[0][i] >= 0 && combinations[1][i] >= 0) {
			parent1genes = get_genes_of_index(d->m, combinations[0][i]);
			parent2genes = get_genes_of_index(d->m, combinations[1][i]);
			if (g.will_track_pedigree) {
				parent1id = get_id_of_index(d->m, combinations[0][i]);
				parent2id = get_id_of_index(d->m, combinations[1][i]);
			}
			
			for (int f = 0; f < g.family_size; ++f, ++fullness) {
				// when cross buffer is full, save these outcomes to the file.
				if (fullness >= 1000) {
					// give the subjects their ids and names
					if (g.will_name_subjects) {
						set_subject_names(crosses, g.subject_prefix, *cross_current_id, 0);
					}
					for (int j = 0; j < 1000; ++j) {
						++ *cross_current_id;
						crosses->ids[j] = *cross_current_id;
					}
					
					// save the subjects to files if appropriate
					if (g.will_save_pedigree_to_file) {
						save_AM_pedigree( fp, crosses, d);
					}
					if (g.will_save_effects_to_file) {
						eff = calculate_fitness_metric( crosses, &(d->e));
						save_fitness( fe, &eff, crosses->ids, crosses->subject_names);
						delete_dmatrix( &eff);
					}
					if (g.will_save_genes_to_file) {
						save_allele_matrix( fg, crosses, d->markers);
					}
					
					if (g.will_save_to_simdata) {
						last->next = crosses;
						last = last->next;
						// get the new crosses matrix, of the right size.
						if (n_to_go < 1000) {
							crosses = create_empty_allelematrix(d->n_markers, n_to_go);
							n_to_go = 0;
						} else {
							crosses = create_empty_allelematrix(d->n_markers, 1000);
							n_to_go -= 1000;
						}
					}
					fullness = 0; //reset the count and start refilling the matrix
				}
				
				
				generate_cross(d, parent1genes, parent2genes, crosses->alleles[fullness]);
				crosses->groups[fullness] = output_group;
				if (g.will_track_pedigree) {
					crosses->pedigrees[0][fullness] = parent1id;
					crosses->pedigrees[1][fullness] = parent2id;
				}
			}	
		}
	}
	PutRNGstate();
	
	// save the rest of the crosses to the file.
	// give the subjects their ids and names
	if (g.will_name_subjects) {
		set_subject_names(crosses, g.subject_prefix, *cross_current_id, 0);
	}
	for (int j = 0; j < fullness; ++j) {
		++ *cross_current_id;
		crosses->ids[j] = *cross_current_id;
	}
	
	// save the subjects to files if appropriate
	if (g.will_save_pedigree_to_file) {
		save_AM_pedigree( fp, crosses, d);
		fclose(fp);
	}
	if (g.will_save_effects_to_file) {
		eff = calculate_fitness_metric( crosses, &(d->e));
		save_fitness( fe, &eff, crosses->ids, crosses->subject_names);
		delete_dmatrix( &eff);
		fclose(fe);
	}
	if (g.will_save_genes_to_file) {
		save_allele_matrix( fg, crosses, d->markers);
		fclose(fg);
	}
	if (g.will_save_to_simdata) {
		last->next = crosses;
		condense_allele_matrix( d ); 
		return output_group;
		
	} else {
		delete_allele_matrix( crosses );
		return 0;
	}
}

/** Selfs each member of a group for a certain number of generations.
 * The resulting genotypes are allocated to a new group.
 *
 * Only the genotype after all n generations is saved. Intermediate steps will be lost.
 *
 * Preferences in GenOptions are applied to this operation. The family_size parameter
 * in GenOptions allows you to generate multiple selfed offspring from each member 
 * of the group. These multiple selfed offspring all originate from independent
 * processes of selfing the parent with itself then selfing that intermediate offspring
 * with itself for the remaining n-1 generations.
 *
 * @param d pointer to the SimData object that contains the genetic map and 
 * genotypes of the parent group.
 * @param n number of generations of selfing simulation to carry out.
 * @param group the genotypes on which to perform these n generations of selfing.
 * @param g options for the genotypes created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
*/
int self_n_times(SimData* d, int n, int group, GenOptions g) {
	int group_size = get_group_size( d, group);
	if (group_size < 1) {
		error("Group %d does not exist.\n", group);
	}
	if (n < 1) {
		error("That number of generations cannot be produced.\n");
	}
	
	AlleleMatrix* outcome;
	int n_to_go = group_size * g.family_size;
	if (n_to_go < 1000) {
		outcome = create_empty_allelematrix(d->n_markers, n_to_go);
		n_to_go = 0;
	} else {
		outcome = create_empty_allelematrix(d->n_markers, 1000);
		n_to_go -= 1000;
	}
	char** group_genes = get_group_genes( d, group, group_size);
	int i, j, f, fullness = 0;

	// set up pedigree/id allocation, if applicable
	unsigned int cid = 0;
	unsigned int* cross_current_id;
	if (g.will_allocate_ids) {
		cross_current_id = &(d->current_id);
	} else {
		cross_current_id = &cid;
	}
	unsigned int* group_ids = NULL;
	if (g.will_track_pedigree) {
		group_ids = get_group_ids( d, group, group_size);
	}
	AlleleMatrix* last = NULL;
	int output_group = 0;
	if (g.will_save_to_simdata) {
		last = d->m; // for saving to simdata
		while (last->next != NULL) {
			last = last->next;
		}
		output_group = get_new_group_num( d);
	}
	
	// open the output files, if applicable
	char fname[100];
	FILE* fp = NULL, * fe = NULL, * fg = NULL;
	DecimalMatrix eff;
	if (g.will_save_pedigree_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-pedigree");
		fp = fopen(fname, "w");
	}
	if (g.will_save_effects_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-eff");
		fe = fopen(fname, "w");
	}
	if (g.will_save_genes_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-genome");
		fg = fopen(fname, "w");
	}

	GetRNGstate();
	for (i = 0; i < group_size; ++i) {	
		R_CheckUserInterrupt();	
		// do n rounds of selfing (j-indexed loops) g.family_size times per individual (f-indexed loops)
		if (n == 1)  {
			//find the parent genes, save a shallow copy to set
			char* genes = group_genes[i];
			for (f = 0; f < g.family_size; ++f, ++fullness) {
				
				// when cross buffer is full, save these outcomes to the file.
				if (fullness >= 1000) {
					outcome->n_subjects = 1000;
					// give the subjects their ids and names
					if (g.will_name_subjects) {
						set_subject_names(outcome, g.subject_prefix, *cross_current_id, 0);
					}
					for (int j = 0; j < 1000; ++j) {
						++ *cross_current_id;
						outcome->ids[j] = *cross_current_id;
					}
					
					// save the subjects to files if appropriate
					if (g.will_save_pedigree_to_file) {
						save_AM_pedigree( fp, outcome, d);
					}
					if (g.will_save_effects_to_file) {
						eff = calculate_fitness_metric( outcome, &(d->e));
						save_fitness( fe, &eff, outcome->ids, outcome->subject_names);
						delete_dmatrix( &eff);
					}
					if (g.will_save_genes_to_file) {
						save_allele_matrix( fg, outcome, d->markers);
					}
					
					if (g.will_save_to_simdata) {
						last->next = outcome;
						last = last->next;
						if (n_to_go < 1000) {
							outcome = create_empty_allelematrix(d->n_markers, n_to_go);
							n_to_go = 0;
						} else {
							outcome = create_empty_allelematrix(d->n_markers, 1000);
							n_to_go -= 1000;
						}
					}
					fullness = 0; //reset the count and start refilling the matrix
				}
				
				generate_cross( d, genes, genes, outcome->alleles[fullness] );
			}
			
		} else {
			//find the parent genes, save a deep copy to set
			char* genes = get_malloc(sizeof(char) * (d->n_markers<<1));
			for (j = 0; j < d->n_markers; ++j) {
				genes[2*j] = group_genes[i][2*j];
				genes[2*j + 1] = group_genes[i][2*j + 1];
			}
			
			for (f = 0; f < g.family_size; ++f, ++fullness) {
				// when cross buffer is full, save these outcomes to the file.
				if (fullness >= 1000) {
					outcome->n_subjects = 1000;
					// give the subjects their ids and names
					if (g.will_name_subjects) {
						set_subject_names(outcome, g.subject_prefix, *cross_current_id, 0);
					}
					for (int j = 0; j < 1000; ++j) {
						++ *cross_current_id;
						outcome->ids[j] = *cross_current_id;
					}
					
					// save the subjects to files if appropriate
					if (g.will_save_pedigree_to_file) {
						save_AM_pedigree( fp, outcome, d);
					}
					if (g.will_save_effects_to_file) {
						eff = calculate_fitness_metric( outcome, &(d->e));
						save_fitness( fe, &eff, outcome->ids, outcome->subject_names);
						delete_dmatrix( &eff);
					}
					if (g.will_save_genes_to_file) {
						save_allele_matrix( fg, outcome, d->markers);
					}
					
					if (g.will_save_to_simdata) {
						last->next = outcome;
						last = last->next;
						if (n_to_go < 1000) {
							outcome = create_empty_allelematrix(d->n_markers, n_to_go);
							n_to_go = 0;
						} else {
							outcome = create_empty_allelematrix(d->n_markers, 1000);
							n_to_go -= 1000;
						}
					}
					fullness = 0; //reset the count and start refilling the matrix
				}
				
				for (j = 0; j < n; ++j) {
					R_CheckUserInterrupt();
					if (j % 2) {
						generate_cross( d, outcome->alleles[fullness], outcome->alleles[fullness], genes);
					} else {
						generate_cross( d, genes, genes, outcome->alleles[fullness] );
					}
				}
				if (n % 2) {
					free(outcome->alleles[fullness]);
					outcome->alleles[fullness] = genes;
					
					// make a new deep copy if we're still going.
					if (f + 1 < g.family_size) {
						genes = get_malloc(sizeof(char) * (d->n_markers<<1));
						for (j = 0; j < d->n_markers; ++j) {
							genes[2*j] = group_genes[i][2*j];
							genes[2*j + 1] = group_genes[i][2*j + 1];
						}
					}
				} else {
					free(genes);
				}
				
				outcome->groups[fullness] = output_group;
				
			}
		}
		
		if (g.will_track_pedigree) {
			fullness -= g.family_size;
			int id = group_ids[i];
			for (f = 0; f < g.family_size; ++f, ++fullness) {
				outcome->pedigrees[0][fullness] = id;
				outcome->pedigrees[1][fullness] = id;
			}
		}
	}
	PutRNGstate();
	
	free(group_genes);
	// save the rest of the crosses to the file.
	outcome->n_subjects = fullness;
	// give the subjects their ids and names
	if (g.will_name_subjects) {
		set_subject_names(outcome, g.subject_prefix, *cross_current_id, 0);
	}
	for (int j = 0; j < fullness; ++j) {
		++ *cross_current_id;
		outcome->ids[j] = *cross_current_id;
	}
	if (g.will_track_pedigree) {
		free(group_ids);
	}
	
	// save the subjects to files if appropriate
	if (g.will_save_pedigree_to_file) {
		save_AM_pedigree( fp, outcome, d);
		fclose(fp);
	}
	if (g.will_save_effects_to_file) {
		eff = calculate_fitness_metric( outcome, &(d->e));
		save_fitness( fe, &eff, outcome->ids, outcome->subject_names);
		delete_dmatrix( &eff);
		fclose(fe);
	}
	if (g.will_save_genes_to_file) {
		save_allele_matrix( fg, outcome, d->markers);
		fclose(fg);
	}
	if (g.will_save_to_simdata) {
		last->next = outcome;
		condense_allele_matrix( d ); 
		return output_group;	
	} else {
		delete_allele_matrix( outcome );
		return 0;
	}
}

/** Creates a doubled haploid from each member of a group.
 * The resulting genotypes are allocated to a new group.
 *
 * Preferences in GenOptions are applied to this operation. The family_size parameter
 * in GenOptions allows you to generate multiple doubled haploid offspring from each member 
 * of the group. These multiple offspring all originate from independent
 * processes of generating a gamete then doubling its alleles.
 *
 * @param d pointer to the SimData object that contains the genetic map and 
 * genotypes of the parent group.
 * @param group the genotypes on which to perform the operation.
 * @param g options for the genotypes created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
*/
int make_doubled_haploids(SimData* d, int group, GenOptions g) {
	int group_size = get_group_size( d, group);
	if (group_size < 1) {
		error("Group %d does not exist.\n", group);
	}
	
	AlleleMatrix* outcome;
	int n_to_go = group_size * g.family_size;
	if (n_to_go < 1000) {
		outcome = create_empty_allelematrix(d->n_markers, n_to_go);
		n_to_go = 0;
	} else {
		outcome = create_empty_allelematrix(d->n_markers, 1000);
		n_to_go -= 1000;
	}
	char** group_genes = get_group_genes( d, group, group_size);
	int i, f, fullness = 0;

	// set up pedigree/id allocation, if applicable
	unsigned int cid = 0;
	unsigned int* cross_current_id;
	if (g.will_allocate_ids) {
		cross_current_id = &(d->current_id);
	} else {
		cross_current_id = &cid;
	}
	unsigned int* group_ids = NULL;
	if (g.will_track_pedigree) {
		group_ids = get_group_ids( d, group, group_size);
	}
	AlleleMatrix* last = NULL;
	int output_group = 0;
	if (g.will_save_to_simdata) {
		last = d->m; // for saving to simdata
		while (last->next != NULL) {
			last = last->next;
		}
		output_group = get_new_group_num( d);
	}
	
	// open the output files, if applicable
	char fname[100];
	FILE* fp = NULL, * fe = NULL, * fg = NULL;
	DecimalMatrix eff;
	if (g.will_save_pedigree_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-pedigree");
		fp = fopen(fname, "w");
	}
	if (g.will_save_effects_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-eff");
		fe = fopen(fname, "w");
	}
	if (g.will_save_genes_to_file) {
		strcpy(fname, g.filename_prefix);
		strcat(fname, "-genome");
		fg = fopen(fname, "w");
	}

	GetRNGstate();
	for (i = 0; i < group_size; ++i) {	
		R_CheckUserInterrupt();	
		// do n rounds of selfing (j-indexed loops) g.family_size times per individual (f-indexed loops)
		//find the parent genes, save a shallow copy to set
		char* genes = group_genes[i];
		int id = group_ids[i];
		for (f = 0; f < g.family_size; ++f, ++fullness) {
			// when cross buffer is full, save these outcomes to the file.
			if (fullness >= 1000) {
				outcome->n_subjects = 1000;
				// give the subjects their ids and names
				if (g.will_name_subjects) {
					set_subject_names(outcome, g.subject_prefix, *cross_current_id, 0);
				}
				for (int j = 0; j < 1000; ++j) {
					++ *cross_current_id;
					outcome->ids[j] = *cross_current_id;
				}
				
				// save the subjects to files if appropriate
				if (g.will_save_pedigree_to_file) {
					save_AM_pedigree( fp, outcome, d);
				}
				if (g.will_save_effects_to_file) {
					eff = calculate_fitness_metric( outcome, &(d->e));
					save_fitness( fe, &eff, outcome->ids, outcome->subject_names);
					delete_dmatrix( &eff);
				}
				if (g.will_save_genes_to_file) {
					save_allele_matrix( fg, outcome, d->markers);
				}
				
				if (g.will_save_to_simdata) {
					last->next = outcome;
					last = last->next;
					if (n_to_go < 1000) {
						outcome = create_empty_allelematrix(d->n_markers, n_to_go);
						n_to_go = 0;
					} else {
						outcome = create_empty_allelematrix(d->n_markers, 1000);
						n_to_go -= 1000;
					}
				}
				fullness = 0; //reset the count and start refilling the matrix
			}
			
			generate_doubled_haploid( d, genes, outcome->alleles[fullness] );
			outcome->groups[fullness] = output_group;
			if (g.will_track_pedigree) {
				outcome->pedigrees[0][fullness] = id;
				outcome->pedigrees[1][fullness] = id;
			}
		}
	}
	PutRNGstate();
	
	free(group_genes);
	// save the rest of the crosses to the file.
	outcome->n_subjects = fullness;
	// give the subjects their ids and names
	if (g.will_name_subjects) {
		set_subject_names(outcome, g.subject_prefix, *cross_current_id, 0);
	}
	for (int j = 0; j < fullness; ++j) {
		++ *cross_current_id;
		outcome->ids[j] = *cross_current_id;
	}
	if (g.will_track_pedigree) {
		free(group_ids);
	}
	
	// save the subjects to files if appropriate
	if (g.will_save_pedigree_to_file) {
		save_AM_pedigree( fp, outcome, d);
		fclose(fp);
	}
	if (g.will_save_effects_to_file) {
		eff = calculate_fitness_metric( outcome, &(d->e));
		save_fitness( fe, &eff, outcome->ids, outcome->subject_names);
		delete_dmatrix( &eff);
		fclose(fe);
	}
	if (g.will_save_genes_to_file) {
		save_allele_matrix( fg, outcome, d->markers);
		fclose(fg);
	}
	if (g.will_save_to_simdata) {
		last->next = outcome;
		condense_allele_matrix( d ); 
		return output_group;	
	} else {
		delete_allele_matrix( outcome );
		return 0;
	}
}

/** Perform crosses between all pairs of parents
 * in the group `from_group` and allocates the resulting offspring to a new group.
 *
 * If the group has n members, there will be n * (n-1) / 2 offspring produced.
 *
 * Preferences in GenOptions are applied to this cross. 
 *
 * @param d pointer to the SimData object containing or markers and parent alleles
 * @param from_group group number from which to do all these crosses.
 * @param g options for the AlleleMatrix created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
 */
int make_all_unidirectional_crosses(SimData* d, int from_group, GenOptions g) {
	int group_size = get_group_size( d, from_group );
	if (group_size < 2) {
		if (group_size == 1) {
			error("Group %d does not have enough members to perform double crosses.\n", from_group);
		} else {
			error("Group %d does not exist.\n", from_group);
		}
	}
	//unsigned int* group_ids = get_group_ids( d, from_group, group_size);
	unsigned int* group_indexes = get_group_indexes( d, from_group, group_size);
	
	// number of crosses = number of entries in upper triangle of matrix
	//    = half of (n entries in matrix - length of diagonal)
	// 	  = half of (lmatrix * lmatrix - lmatrix);
	int n_crosses = group_size * (group_size - 1) / 2; //* g.family_size;
	
	int combinations[2][n_crosses];
	int cross_index = 0;
	for (int i = 0; i < group_size; ++i) {
		for (int j = i + 1; j < group_size; ++j) {
			combinations[0][cross_index] = group_indexes[i];
			combinations[1][cross_index] = group_indexes[j];
			
			++cross_index;
		}
	}
	
	return cross_these_combinations(d, n_crosses, combinations, g);
	
}

/** Find the top m percent of a group and perform random crosses between those
 * top individuals. The resulting offspring are allocated to a new group.
 * 
 * Preferences in GenOptions are applied to this cross.
 *
 * @param d pointer to the SimData object containing or markers and parent alleles
 * @param n number of random crosses to make.
 * @param m percentage (as a decimal, i.e. 0.2 for 20percent). Take the best [this portion]
 * of the group for performing the random crosses.
 * @param group group number from which to identify top parents and do crosses
 * @param g options for the AlleleMatrix created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
 */
int make_n_crosses_from_top_m_percent(SimData* d, int n, int m, int group, GenOptions g) {
	// move the top m% to a new group
	int group_size = get_group_size(d, group);
	int n_top_group = group_size * m / 100; //@integer division?
	printf("There are %d lines in the top %d%%\n", n_top_group, m);
	
	int topgroup = split_group_by_fitness(d, group, n_top_group, FALSE);
	
	// do the random crosses
	int gp = cross_random_individuals(d, topgroup, n, g);

	// unconvert from a group
	int to_combine[] = {group, topgroup};
	combine_groups(d, 2, to_combine);
	
	return gp;
}
		
/** Perform crosses between pairs of parents identified by name in a file
 * and allocate the resulting offspring to a new group.
 *
 * The input file should have format:
 *
 * [parent1 name] [parent2 name]
 *
 * [parent1 name] [parent2 name]
 *
 * ...
 *
 * where each row represents a separate cross to carry out.
 *
 * Preferences in GenOptions are applied to this cross. 
 *
 * @param d pointer to the SimData object containing or markers and parent alleles
 * @param input_file file instructing which crosses to perform
 * @param g options for the AlleleMatrix created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
 */
int make_crosses_from_file(SimData* d, const char* input_file, GenOptions g) {
	struct TableSize t = get_file_dimensions(input_file, '\t');
	if (t.num_rows < 1) {
		error("No crosses exist in that file\n");
	}
	
	//open file
	FILE* fp;
	if ((fp = fopen(input_file, "r")) == NULL) {
		error( "Failed to open file %s.\n", input_file);
	}
	
	int combinations[2][t.num_rows];
	char buffer[2][50];
	// for each row in file
	for (int i = 0; i < t.num_rows; ++i) {
		// load the four grandparents
		fscanf(fp, "%s %s \n", buffer[0], buffer[1]);
		combinations[0][i] = get_index_of_name(d->m, buffer[0]);
		combinations[1][i] = get_index_of_name(d->m, buffer[1]);		
	}
	
	fclose(fp);
	return cross_these_combinations(d, t.num_rows, combinations, g);
}
	
/** Perform crosses between previously-generated offspring of pairs of parents 
 * identified by name in a file. The resulting offspring are allocated to a new
 * group.
 *
 * The input file should have format:
 *
 * [gparent1 name] [gparent2 name] [gparent3 name] [gparent4 name]
 *
 * [gparent1 name] [gparent2 name] [gparent3 name] [gparent4 name]
 *
 * ...
 *
 * where each row represents a separate cross to carry out.
 *
 * Results of the cross [gparent1 name] x [gparent2 name] and 
 * [gparent3 name] x [gparent4 name] must have already been generated 
 * with pedigree tracking options turned on. Messages will be printed 
 * to stderr if such offspring cannot be found.
 *
 * Preferences in GenOptions are applied to this cross. 
 *
 * @param d pointer to the SimData object containing or markers and parent alleles
 * @param input_file file instructing which crosses to perform
 * @param g options for the AlleleMatrix created. @see GenOptions
 * @returns the group number of the group to which the produced offspring were allocated.
 */
int make_double_crosses_from_file(SimData* d, const char* input_file, GenOptions g) {
	
	struct TableSize t = get_file_dimensions(input_file, '\t');
	if (t.num_rows < 1) {
		error("No crosses exist in that file\n");
	}
	
	//open file
	FILE* fp;
	if ((fp = fopen(input_file, "r")) == NULL) {
		error( "Failed to open file %s.\n", input_file);
	}
	
	int combinations[2][t.num_rows];
	char buffer[4][50];
	char* to_buffer[] = {buffer[0], buffer[1], buffer[2], buffer[3]};
	unsigned int g0_id[4];
	int f1_i[2];
	// for each row in file
	for (int i = 0; i < t.num_rows; ++i) {
		// load the four grandparents
		fscanf(fp, "%s %s %s %s \n", buffer[0], buffer[1], buffer[2], buffer[3]);
		get_ids_of_names(d->m, 4, to_buffer, g0_id);
		if (g0_id[0] < 0 || g0_id[1] < 0 || g0_id[2] < 0 || g0_id[3] < 0) {
			warning("Could not go ahead with the line %d cross - g0 names not in records\n", i);
			combinations[0][i] = -1;
			combinations[1][i] = -1;
			continue;
		}
		
		// identify two parents
		f1_i[0] = get_index_of_child(d->m, g0_id[0], g0_id[1]);
		f1_i[1] = get_index_of_child(d->m, g0_id[2], g0_id[3]);
		if (f1_i[0] < 0 || f1_i[1] < 0) {
			// try different permutations of the four grandparents.
			f1_i[0] = get_id_of_child(d->m, g0_id[0], g0_id[2]);
			f1_i[1] = get_id_of_child(d->m, g0_id[1], g0_id[3]);
			if (f1_i[0] < 0 || f1_i[1] < 0) {
				f1_i[0] = get_index_of_child(d->m, g0_id[0], g0_id[3]);
				f1_i[1] = get_index_of_child(d->m, g0_id[1], g0_id[2]);
				if (f1_i[0] < 0 || f1_i[1] < 0) {
					warning("Could not go ahead with the line %d cross - f1 children do not exist for this quartet\n", i);
					combinations[0][i] = -1;
					combinations[1][i] = -1;
					continue;
				}
			}
		}
	
		//add them to a combinations list
		combinations[0][i] = f1_i[0];
		combinations[1][i] = f1_i[1];
		
	}
	
	fclose(fp);
	return cross_these_combinations(d, t.num_rows, combinations, g);
	
}
