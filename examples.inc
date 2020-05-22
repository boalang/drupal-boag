<?php

global $examples, $example_options;

if (!function_exists('t')) {
	function t($str) { return $str; }
}

# ------------------------------------------------------------------------------------ #

$example_names['q1'] = t('What are the number of taxonomic assignment for each protein sequence in the NR database?');
$example_jobs['q1'] = 1;
$examples['q1'] = <<<BOA
s: Sequence = input;
counts: output sum[string][string] of int;
foreach(i:int; def(s.annotation[i]))
	counts [s.seqid][s.annotation[i].tax_name] <<  1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q2'] = t('What are the list of conserved proteins?');
$example_jobs['q2'] = 2;
$examples['q2'] = <<<BOA
s: Sequence = input;
protOut : output sum [string] of int;
# We defined conserved as proteins that have more than 10 distinct taxa
if (len(s.annotation) > 10){
  foreach(i:int; def(s.annotation[i]))
   protOut [s.annotation[i].defline] << 1;
}
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q3'] = t('What are the frequencies of taxonomic assignments in clusters with different similarity level?');
$example_jobs['q3'] = 3;
$examples['q3'] = <<<BOA
s: Sequence = input;
clstrOut : output sum [int][string][string] of int;
foreach(i:int; def(s.annotation[i]))
   foreach(j:int; def(s.cluster[j]))
   	 clstrOut[s.cluster[j].similarity][s.cluster[j].cid] [s.annotation[i].tax_name]<< 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q4'] = t('What is the frequency of protein length in the NR database?');
$example_jobs['q4'] = 4;
$examples['q4'] = <<<BOA
s: Sequence = input;
counts: output sum[int] of int;
foreach(i:int; def(s.cluster[i]))
  if (s.cluster[i].similarity==95)
	counts [s.cluster[i].length] <<  1;

BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q5'] = t('What are all clusters belong to a specific protein function SCN?');
$example_jobs['q5'] = 5;
$examples['q5'] = <<<BOA
s: Sequence = input;
counts_protein: output collection[string][string] of int;

foreach(i:int; def(s.cluster[i])){
  if (s.cluster[i].similarity==95){
    foreach(j:int; def(s.annotation[j]))
			if (match(`SCN`,s.annotation[j].defline))
			  counts_protein [s.seqid][s.cluster[i].cid] <<1;
		  }
	}
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['q6'] = t('What are number of protiens in each phylum in the tree of life?');
$example_jobs['q6'] = 6;
$examples['q6'] = <<<BOA
# search for Streptococcus
s: Sequence = input;
phylCount: output sum [string] of int;

taxs := {
    "Anthocerotophyta","Bryophyta", "Magnoliophyta","Pinophyta", "Coniferophyta","Pteridophyta", "Rhodophyta",
    "Amoebozoa", "Bigyra","Cercozoa", "Choanozoa","Ciliophora", "Cryptista","Euglenozoa", "Foraminifera",
    "Acidobacteria", "Actinobacteria","Aquificae", "Armatimonadetes",
    "Chloroflexi", "Chrysiogenetes","Cyanobacteria", "Deferribacteres","Deinococcus-Thermus",
    "Fibrobacteres","Firmicutes", "Fusobacteria","Gemmatimonadetes", "Lentisphaerae",
};
for (j := 0; j < len(taxs); j++)
	exists(i: int; match(taxs[j], s.annotation[i].tax_name))
	 	 phylCount [taxs[j]] << 1;
BOA;
# ------------------------------------------------------------------------------------ #

$example_names['q7'] = t('What are the list of taxonomic assignment for all proteins in NR?');
$example_jobs['q7'] = 31;
$examples['q7'] = <<<BOA
s: Sequence = input;
clstrOut : output collection [string] of string;
getTaxList := function(seq: Sequence):string {
taxids :="";
foreach(i:int; def(s.annotation[i])){
    if (s.annotation[i].tax_name !="")
    taxids = taxids + s.annotation[i].tax_id + " ";}
    return taxids ;
};
foreach(i:int; def(s.cluster[i]))
    if (s.cluster[i].similarity==95 && s.cluster[i].representative)
        clstrOut [s.seqid] <<  getTaxList(s);
BOA;


# ------------------------------------------------------------------------------------ #

$example_names['q8'] = t('What are proteins that do not have a taxonomic assignment?');
$example_jobs['q8'] = 8;
$examples['q8'] = <<<BOA
s: Sequence = input;
countNull : output sum of int;
if (s.annotation[i].tax_name =="")
  countNull  <<1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_options = array(
	 0 => t('-- Select Example --'),
	 'q1' => $example_names['q1'],
	 'q2' => $example_names['q2'],
	 'q3' => $example_names['q3'],
	 'q4' => $example_names['q4'],
	 'q5' => $example_names['q5'],
	 'q6' => $example_names['q6'],
	 'q7' => $example_names['q7'],
	 'q8' => $example_names['q8'],
);

?>
