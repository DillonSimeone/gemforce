#ifndef _CRIT_UTILS_H
#define _CRIT_UTILS_H

struct Gem_Y {
	int grade;				//using short does NOT improve time/memory usage
	float damage;
	float crit;
	struct Gem_Y* father;
	struct Gem_Y* mother;
};

// --------------------
// Common gem interface
// --------------------

#include "gem_stats.h"

void gem_print_Y(gemY *p_gem) {
	printf("Grade:\t%d\nDamage:\t%f\nCrit:\t%f\nPower:\t%f\n\n",
		p_gem->grade, p_gem->damage, p_gem->crit, p_gem->damage*p_gem->crit);
}

// -----------------
// Combining section
// -----------------

void gem_comb_eq_Y(gemY *p_gem1, gemY *p_gem2, gemY *p_gem_combined)
{
	p_gem_combined->grade = p_gem1->grade+1;
	if (p_gem1->damage > p_gem2->damage) p_gem_combined->damage = DAMAGE_EQ_1*p_gem1->damage + DAMAGE_EQ_2*p_gem2->damage;
	else p_gem_combined->damage = DAMAGE_EQ_1*p_gem2->damage + DAMAGE_EQ_2*p_gem1->damage;
	if (p_gem1->crit > p_gem2->crit) p_gem_combined->crit = CRIT_EQ_1*p_gem1->crit + CRIT_EQ_2*p_gem2->crit;
	else p_gem_combined->crit = CRIT_EQ_1*p_gem2->crit + CRIT_EQ_2*p_gem1->crit;
}

void gem_comb_d1_Y(gemY *p_gem1, gemY *p_gem2, gemY *p_gem_combined)     //bigger is always gem1
{
	p_gem_combined->grade = p_gem1->grade;
	if (p_gem1->damage > p_gem2->damage) p_gem_combined->damage = DAMAGE_D1_1*p_gem1->damage + DAMAGE_D1_2*p_gem2->damage;
	else p_gem_combined->damage = DAMAGE_D1_1*p_gem2->damage + DAMAGE_D1_2*p_gem1->damage;
	if (p_gem1->crit > p_gem2->crit) p_gem_combined->crit = CRIT_D1_1*p_gem1->crit + CRIT_D1_2*p_gem2->crit;
	else p_gem_combined->crit = CRIT_D1_1*p_gem2->crit + CRIT_D1_2*p_gem1->crit;
}

void gem_comb_gn_Y(gemY *p_gem1, gemY *p_gem2, gemY *p_gem_combined)
{
	p_gem_combined->grade = int_max(p_gem1->grade, p_gem2->grade);
	if (p_gem1->damage > p_gem2->damage) p_gem_combined->damage = DAMAGE_GN_1*p_gem1->damage + DAMAGE_GN_2*p_gem2->damage;
	else p_gem_combined->damage = DAMAGE_GN_1*p_gem2->damage + DAMAGE_GN_2*p_gem1->damage;
	if (p_gem1->crit > p_gem2->crit) p_gem_combined->crit = CRIT_GN_1*p_gem1->crit + CRIT_GN_2*p_gem2->crit;
	else p_gem_combined->crit = CRIT_GN_1*p_gem2->crit + CRIT_GN_2*p_gem1->crit;
}

void gem_combine_Y (gemY *p_gem1, gemY *p_gem2, gemY *p_gem_combined)
{
	p_gem_combined->father=p_gem1;
	p_gem_combined->mother=p_gem2;
	int delta = p_gem1->grade - p_gem2->grade;
	switch (delta){
		case 0:
			gem_comb_eq_Y(p_gem1, p_gem2, p_gem_combined);
			break;
		case 1:
			gem_comb_d1_Y(p_gem1, p_gem2, p_gem_combined);
			break;
		case -1:
			gem_comb_d1_Y(p_gem2, p_gem1, p_gem_combined);
			break;
		default:
			gem_comb_gn_Y(p_gem1, p_gem2, p_gem_combined);
			break;
	}
	if (p_gem_combined->damage < p_gem1->damage) p_gem_combined->damage = p_gem1->damage;
	if (p_gem_combined->damage < p_gem2->damage) p_gem_combined->damage = p_gem2->damage;
}

void gem_init_Y(gemY *p_gem, int grd, float damage, float crit)
{
	p_gem->grade =grd;
	p_gem->damage=damage;
	p_gem->crit  =crit;
	p_gem->father=NULL;
	p_gem->mother=NULL;
}

// ------------------------
// Redefine pool_from_table
// ------------------------

#define GEM_SUFFIX Y
#include "gfon.h"
#undef GEM_SUFFIX

// ---------------
// Sorting section
// ---------------

inline int gem_has_less_damage_crit(gemY gem1, gemY gem2)
{
	if (gem1.damage < gem2.damage) return 1;
	else if (gem1.damage == gem2.damage && gem1.crit < gem2.crit) return 1;
	else return 0;
}

void ins_sort_Y (gemY* gems, int len)
{
	int i,j;
	gemY element;
	for (i=1; i<len; i++) {
		element=gems[i];
		for (j=i; j>0 && gem_has_less_damage_crit(element, gems[j-1]); j--) {
			gems[j]=gems[j-1];
		}
		gems[j]=element;
	}
}

void quick_sort_Y (gemY* gems, int len)
{
	if (len > 20)  {
		gemY pivot = gems[len/2];
		gemY* beg = gems;
		gemY* end = gems+len-1;
		while (beg <= end) {
			while (gem_has_less_damage_crit(*beg, pivot)) {
				beg++;
			}
			while (gem_has_less_damage_crit(pivot,*end)) {
				end--;
			}
			if (beg <= end) {
				gemY temp = *beg;
				*beg = *end;
				*end = temp;
				beg++;
				end--;
			}
		}
		if (end-gems+1 < gems-beg+len) {		// sort smaller first
			quick_sort_Y(gems, end-gems+1);
			quick_sort_Y(beg, gems-beg+len);
		}
		else {
			quick_sort_Y(beg, gems-beg+len);
			quick_sort_Y(gems, end-gems+1);
		}
	}
}

void gem_sort_Y (gemY* gems, int len)
{
	quick_sort_Y (gems, len);		// partially sort
	ins_sort_Y (gems, len);			// finish the nearly sorted array
}

// ---------------------------
// Redefine printing functions
// ---------------------------

void print_parens_Y(gemY* gemf)
{
	if (gemf->grade==0) printf("-");
	else if (gemf->father==NULL) printf("%c", COLOR_CRIT);
	else {
		printf("(");
		print_parens_Y(gemf->mother);
		printf("+");
		print_parens_Y(gemf->father);
		printf(")");
	}
	return;
}

int gem_getvalue_Y(gemY* p_gem)
{
	if (p_gem->grade==0) return 0;
	if (p_gem->father==NULL) return 1;
	else return gem_getvalue_Y(p_gem->father)+gem_getvalue_Y(p_gem->mother);
}

void print_parens_compressed_Y(gemY* gemf)
{
	if (gemf->grade==0) printf("-");
	else if (gemf->father==NULL) {
		printf("%c", COLOR_CRIT);
	}
	else if (pow(2,gemf->grade-1)==gem_getvalue_Y(gemf)) {				// if gem is standard combine
		printf("%d%c", gemf->grade, COLOR_CRIT);
	}
	else {
		printf("(");
		print_parens_compressed_Y(gemf->mother);
		printf("+");
		print_parens_compressed_Y(gemf->father);
		printf(")");
	}
}

void fill_array_Y(gemY* gemf, gemY** p_gems, int* uniques)
{
	if (gemf->father != NULL) {
		fill_array_Y(gemf->father, p_gems, uniques);
		fill_array_Y(gemf->mother, p_gems, uniques);
	}
	
	for (int i=0; i<*uniques; ++i)
		if (gemf==p_gems[i]) return;
	
	p_gems[*uniques]=gemf;
	(*uniques)++;
}

void print_equations_Y(gemY* gemf)
{
	if (gemf->grade==0) {
		printf("-\n");
		return;
	}
	// fill
	int value=gem_getvalue_Y(gemf);
	int len=2*value-1;
	gemY** p_gems = malloc(len*sizeof(gemY*));		// stores all the gem pointers
	int uniques = 0;
	fill_array_Y(gemf, p_gems, &uniques);			// this array contains marked uniques only and is long `uniques`
	
	// mark
	int orig_grades[uniques];		// stores all the original gem grades
	for (int i = 0; i < uniques; i++) {
		gemY* p_gem = p_gems[i];
		orig_grades[i] = p_gem->grade;
		p_gem->grade = i + 1; // grade must not be 0
	}
	
	// print
	for (int i = 0; i < uniques; i++) {
		gemY* p_gem = p_gems[i];
		if (p_gem->father == NULL)
			printf("(val = 1)\t%2d = g1 %c\n", p_gem->grade - 1, COLOR_CRIT);
		else
			printf("(val = %d)\t%2d = %2d + %2d\n", gem_getvalue_Y(p_gem), p_gem->grade - 1, p_gem->mother->grade - 1, p_gem->father->grade - 1);
	}
	
	// clean
	for (int i = 0; i < uniques; i++) {
		p_gems[i]->grade = orig_grades[i];
	}
	free(p_gems);
}

void print_tree_Y(gemY* gemf, const char* prefix)
{
	if (gemf->grade==0) printf("-\n");
	else if (gemf->father==NULL) {
		printf("─ g1 %c\n", COLOR_CRIT);
	}
	else {
		printf("─%d\n",gem_getvalue_Y(gemf));
		printf("%s ├",prefix);
		char string1[strlen(prefix)+5];  // 1 space, 1 unicode bar and and the null term are 5 extra chars
		sprintf(string1, "%s │", prefix);
		print_tree_Y(gemf->mother, string1);
		
		printf("%s └",prefix);
		char string2[strlen(prefix)+3];  // 2 spaces and the null term are 3 extra chars
		sprintf(string2, "%s  ", prefix);
		print_tree_Y(gemf->father, string2);
	}
}

#endif // _CRIT_UTILS_H
