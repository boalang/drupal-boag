<?php

global $examples, $example_options;

if (!function_exists('t')) {
	function t($str) { return $str; }
}

# ------------------------------------------------------------------------------------ #

$example_names['scheme-use'] = t('How many projects use the Scheme programming language?');
$example_jobs['scheme-use'] = 547;
$examples['scheme-use'] = <<<BOA
# Counting projects using Scheme
p: Project = input;
counts: output sum of int;

foreach (i: int; match(`^scheme$`, lowercase(p.programming_languages[i])))
	counts &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['top-langs'] = t('What are the ten most used programming languages?');
$example_jobs['top-langs'] = 545;
$examples['top-langs'] = <<<BOA
# Counting the 10 most used programming languages
p: Project = input;
counts: output top(10) of string weight int;

foreach (i: int; def(p.programming_languages[i]))
	counts &lt;&lt; p.programming_languages[i] weight 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['mult-langs'] = t('How many projects use more than one programming language?');
$example_jobs['mult-langs'] = 546;
$examples['mult-langs'] = <<<BOA
# Counting the number of projects written in more than one languages
p: Project = input;
counts: output sum of int;

if (len(p.programming_languages) &gt; 1)
	counts &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['java-added'] = t('In which year was Java added to SVN projects the most?');
$example_jobs['java-added'] = 0;
$examples['java-added'] = <<<BOA
# In what year was Java added the most?
p: Project = input;
counts: output sum[int] of int;

HasJavaFile := function(rev: Revision): bool {
	exists (i: int; match(`\.java$`, rev.files[i].name))
		return true;
	return false;
};

foreach (i: int; def(p.code_repositories[i]))
	exists (j: int; HasJavaFile(p.code_repositories[i].revisions[j]))
		counts[yearof(p.code_repositories[i].revisions[j].commit_date)] &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['proj-created'] = t('How many projects are created each year?');
$example_jobs['proj-created'] = 548;
$examples['proj-created'] = <<<BOA
# How many projects created each year?
p: Project = input;
counts: output sum[int] of int;

counts[yearof(p.created_date)] &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['proj-topics'] = t('How many projects self-classify into each topic provided by SourceForge?');
$example_jobs['proj-topics'] = 549;
$examples['proj-topics'] = <<<BOA
# how many projects self-classify into each topic?
p: Project = input;
values: output sum[string] of int;

foreach (i: int; def(p.topics[i]))
	values[lowercase(p.topics[i])] &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['active-proj'] = t('How many Java projects using SVN were active in 2011?');
$example_jobs['active-proj'] = 551;
$examples['active-proj'] = <<<BOA
# Counting the number of active Java projects with SVN
p: Project = input;
counts: output sum of int;

visit(p, visitor {
	before n: Project -&gt; ifall (i: int; !match(`^java$`, lowercase(n.programming_languages[i]))) stop;
	before node: CodeRepository -&gt;
		if (node.kind == RepositoryKind.SVN)
			exists (j: int; yearof(node.revisions[j].commit_date) == 2011)
				counts &lt;&lt; 1;
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['svn-added'] = t('In which year was SVN added to Java projects the most?');
$example_jobs['svn-added'] = 550;
$examples['svn-added'] = <<<BOA
# which year were SVN projects added most
p: Project = input;
counts: output top(1) of int weight int;

visit(p, visitor {
	before n: Project -&gt; ifall (i: int; !match(`^java$`, lowercase(n.programming_languages[i]))) stop;
	before node: CodeRepository -&gt;
		if (node.kind == RepositoryKind.SVN && len(node.revisions) &gt; 0)
			counts &lt;&lt; yearof(node.revisions[0].commit_date) weight 1;
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['num-revisions'] = t('How many revisions are there in all Java projects using SVN?');
$example_jobs['num-revisions'] = 552;
$examples['num-revisions'] = <<<BOA
# Counting the number of revisions for all Java projects with SVN
p: Project = input;
counts: output sum of int;

visit(p, visitor {
	before n: Project -&gt; ifall (i: int; !match(`^java$`, lowercase(n.programming_languages[i]))) stop;
	before node: CodeRepository -&gt; if (node.kind == RepositoryKind.SVN) counts &lt;&lt; len(node.revisions);
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['fix-bugs'] = t('How many revisions fix bugs in all Java projects using SVN?');
$example_jobs['fix-bugs'] = 553;
$examples['fix-bugs'] = <<<BOA
# Counting the number of fixing revisons for all Java projects with SVN
p: Project = input;
counts: output sum of int;

visit(p, visitor {
	before n: Project -&gt; ifall (i: int; !match(`^java$`, lowercase(n.programming_languages[i]))) stop;
	before node: CodeRepository -&gt; if (node.kind != RepositoryKind.SVN) stop;
	before node: Revision -&gt; if (isfixingrevision(node.log)) counts &lt;&lt; 1;
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['committers-proj'] = t('How many committers are there for each project?');
$example_jobs['committers-proj'] = 695;
$examples['committers-proj'] = <<<BOA
# How many committers are in each project?
p: Project = input;
counts: output sum[string] of int;

committers: map[string] of bool;

visit(p, visitor {
	before node: Revision -&gt;
		if (!haskey(committers, node.committer.username)) {
			committers[node.committer.username] = true;
			counts[p.id] &lt;&lt; 1;
		}
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['proj-committers'] = t('How many projects does each committer work on?');
$example_jobs['proj-committers'] = 0;
$examples['proj-committers'] = <<<BOA
# How many projects does each committer work on?
p: Project = input;
counts: output sum[string] of int;
committers: map[string] of bool;

foreach (i: int; def(p.code_repositories[i]))
	foreach (j: int; def(p.code_repositories[i].revisions[j]))
		committers[p.code_repositories[i].revisions[j].committer.username] = true;

keys := keys(committers);
foreach (i: int; def(keys[i]))
	counts[keys[i]] &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['churn-rate'] = t('What are the churn rates for all projects?');
$example_jobs['churn-rate'] = 555;
$examples['churn-rate'] = <<<BOA
# what are the churn rates for all projects
p: Project = input;
counts: output mean[string] of int;

visit(p, visitor {
	before node: Revision -&gt; counts[p.id] &lt;&lt; len(node.files);
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['commits-years'] = t('How did the number of commits for Java projects using SVN change over years?');
$example_jobs['commits-years'] = 556;
$examples['commits-years'] = <<<BOA
# how did # of commits for Java/SVN change over time?
p: Project = input;
counts: output sum[int] of int;

visit(p, visitor {
	before n: Project -&gt; ifall (i: int; !match(`^java$`, lowercase(n.programming_languages[i]))) stop;
	before n: CodeRepository -&gt; if (n.kind != RepositoryKind.SVN) stop;
	before n: Revision -&gt; counts[yearof(n.commit_date)] &lt;&lt; 1;
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['word-dist'] = t('For all SVN revisions in Java projects, what is the distribution of commit log length?');
$example_jobs['word-dist'] = 0;
$examples['word-dist'] = <<<BOA
# distribution of commit log words over revisions
p: Project = input;
counts: output sum[int] of int;

foreach (i: int; def(p.code_repositories[i]))
	foreach (j: int; def(p.code_repositories[i].revisions[j]))
		counts[len(splitall(p.code_repositories[i].revisions[j].log, `\s`))] &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['top-license'] = t('What are the five most used licenses?');
$example_jobs['top-license'] = 557;
$examples['top-license'] = <<<BOA
# Counting the 5 most frequently used licenses
p: Project = input;
counts: output top(5) of string weight int;

foreach (i: int; def(p.licenses[i]))
	counts &lt;&lt; p.licenses[i] weight 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['mult-license'] = t('How many projects use more than one license?');
$example_jobs['mult-license'] = 558;
$examples['mult-license'] = <<<BOA
# Counting the number of projects using more than 1 license
p: Project = input;
counts: output sum of int;

if (len(p.licenses) &gt; 1)
	counts &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['top-os'] = t('What are the five most supported operating systems?');
$example_jobs['top-os'] = 559;
$examples['top-os'] = <<<BOA
# what are the 5 most supported OSes?
p: Project = input;
counts: output top(5) of string weight int;

foreach (i: int; def(p.operating_systems[i]))
	counts &lt;&lt; p.operating_systems[i] weight 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['mult-os'] = t('Which projects support multiple operating systems?');
$example_jobs['mult-os'] = 560;
$examples['mult-os'] = <<<BOA
# which projects support multiple OSes?
p: Project = input;
counts: output collection[string] of string;

if (len(p.operating_systems) &gt; 1)
	counts[p.id] &lt;&lt; p.project_url;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['top-db'] = t('What are the five most popular databases?');
$example_jobs['top-db'] = 561;
$examples['top-db'] = <<<BOA
# what are the 5 most popular databases?
p: Project = input;
counts: output top(5) of string weight int;

foreach (i: int; def(p.databases[i]))
	counts &lt;&lt; p.databases[i] weight 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['mult-db'] = t('What are the projects that support multiple databases?');
$example_jobs['mult-db'] = 562;
$examples['mult-db'] = <<<BOA
# which projects support multiple databases?
p: Project = input;
counts: output collection[string] of string;

if (len(p.databases) &gt; 1)
	counts[p.id] &lt;&lt; p.name;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['db-pl'] = t('How often is each database used in each programming language?');
$example_jobs['db-pl'] = 563;
$examples['db-pl'] = <<<BOA
# pairs of programming language/database
p: Project = input;
counts: output sum[string][string] of int;

foreach (i: int; def(p.programming_languages[i]))
	foreach (j: int; def(p.databases[j]))
		counts[p.programming_languages[i]][p.databases[j]] &lt;&lt; 1;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['ast-count'] = t('What are the five largest projects, in terms of AST nodes?');
$example_jobs['ast-count'] = 564;
$examples['ast-count'] = <<<BOA
# What are the 5 largest projects, in terms of AST nodes?
# Output is in Millions of AST nodes.
p: Project = input;
top5: output top(5) of string weight int;

astCount := 0;

visit(p, visitor {
	# only look at the latest snapshot
	before n: CodeRepository -&gt; {
		snapshot := getsnapshot(n);
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	# by default, count all visited nodes
	before _ -&gt; astCount++;
	# these nodes are not part of the AST, so do nothing when visiting
	before Project, ChangedFile -&gt; ;
});

top5 &lt;&lt; p.project_url weight astCount / 1000000;
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['java-count'] = t('How many valid Java files in latest snapshot?');
$example_jobs['java-count'] = 565;
$examples['java-count'] = <<<BOA
# count how many valid Java files are in the latest snapshot
p: Project = input;
counts: output sum of int;

visit(p, visitor {
	before node: CodeRepository -&gt;
		counts &lt;&lt; len(getsnapshot(node, "SOURCE_JAVA_JLS"));
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['null-check'] = t('How many fixing revisions added null checks?');
$example_jobs['null-check'] = 63199;
$examples['null-check'] = <<<BOA
# How many fixing revisions added null checks?
AddedNullCheck: output sum of int;
p: Project = input;

isfixing := false;
count := 0;
# map of file names to the last revision of that file
files: map[string] of ChangedFile;

visit(p, visitor {
	before node: Revision -&gt; isfixing = isfixingrevision(node.log);
	before node: ChangedFile -&gt; {
		# if this is a fixing revision and
		# there was a previous version of the file
		if (isfixing && haskey(files, node.name)) {
			# count how many null checks were previously in the file
			count = 0;
			visit(getast(files[node.name]));
			last := count;

			# count how many null checks are currently in the file
			count = 0;
			visit(getast(node));

			# if there are more null checks, output
			if (count &gt; last)
				AddedNullCheck &lt;&lt; 1;
		}
		if (node.change == ChangeKind.DELETED)
			remove(files, node.name);
		else
			files[node.name] = node;
		stop;
	}
	before node: Statement -&gt;
		# increase the counter if there is an IF statement
		# where the boolean condition is of the form:
		#   null == expr OR expr == null OR null != expr OR expr != null
		if (node.kind == StatementKind.IF)
			visit(node.expression, visitor {
				before node: Expression -&gt;
					if (node.kind == ExpressionKind.EQ || node.kind == ExpressionKind.NEQ)
						exists (i: int; isliteral(node.expressions[i], "null"))
							count++;
			});
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['dead-code'] = t('What files have unreachable statements?');
$example_jobs['dead-code'] = 3148;
$examples['dead-code'] = <<<BOA
# looking for dead code
DEAD: output top(1000000) of string weight int;

cur_file: string;
cur_method: string;
s: stack of bool;
alive := true;

visit(input, visitor {
	before _ -&gt; if (!alive) stop;
	before node: CodeRepository -&gt; {
		snapshot := getsnapshot(node);
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	before node: ChangedFile -&gt; cur_file = string(node);
	before node: Method -&gt; {
		cur_method = node.name;
		push(s, alive);
		alive = true;
	}
	after node: Method -&gt; alive = pop(s);
	before node: Statement -&gt; {
		if (!alive) {
			DEAD &lt;&lt; format("%s - %s", cur_file, cur_method) weight 1;
			stop;
		}
		switch (node.kind) {
			case StatementKind.BREAK: if (def(node.expression)) break;
			case StatementKind.RETURN, StatementKind.THROW, StatementKind.CONTINUE:
				alive = false;
				break;
			case StatementKind.IF, StatementKind.LABEL: stop;
			case StatementKind.FOR, StatementKind.DO, StatementKind.WHILE,
					StatementKind.SWITCH, StatementKind.TRY:
				foreach (i: int; def(node.statements[i])) {
					push(s, alive);
					visit(node.statements[i]);
					alive = pop(s);
				}
				stop;
			default:
				break;
		}
	}
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['gen-fields'] = t('How many generic fields are declared in each project?');
$example_jobs['gen-fields'] = 600;
$examples['gen-fields'] = <<<BOA
# How many generic fields are declared in each project?
p: Project = input;
GenericFields: output sum[string] of int;

visit(p, visitor {
	before node: Type -&gt;
		if (strfind("&lt;", node.name) &gt; -1)
			GenericFields[p.project_url] &lt;&lt; 1;

	before node: Declaration -&gt; {
		# check all fields
		foreach (i: int; node.fields[i])
			visit(node.fields[i]);

		# also look at nested declarations
		foreach (i: int; node.methods[i])
			visit(node.methods[i]);
		foreach (i: int; node.nested_declarations[i])
			visit(node.nested_declarations[i]);
		stop;
	}
	before node: Method -&gt; {
		foreach (i: int; node.statements[i])
			visit(node.statements[i]);
		stop;
	}
	before node: Statement -&gt; {
		foreach (i: int; node.statements[i])
			visit(node.statements[i]);
		if (def(node.type_declaration))
			visit(node.type_declaration);
		stop;
	}

	# fields cant be below expressions or modifiers
	before Expression, Modifier -&gt; stop;
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['varargs'] = t('How is varargs used over time?');
$example_jobs['varargs'] = 601;
$examples['varargs'] = <<<BOA
# How is varargs used over time?
p: Project = input;
Varargs: output collection[string][string][time] of int;

file_name: string;
commit_date: time;

visit(p, visitor {
	before node: ChangedFile -&gt; file_name = node.name;
	before node: Revision -&gt; commit_date = node.commit_date;
	before node: Method -&gt;
		if (len(node.arguments) &gt; 0
				&& strfind("...", node.arguments[len(node.arguments) - 1].variable_type.name) &gt; -1)
			Varargs[p.project_url][file_name][commit_date] &lt;&lt; 1;
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['transient'] = t('How is transient keyword used in Java?');
$example_jobs['transient'] = 602;
$examples['transient'] = <<<BOA
# How is transient keyword used in Java?
p: Project = input;
TransientTotal: output sum of int;
TransientMax: output maximum(1) of string weight int;
TransientMin: output minimum(1) of string weight int;
TransientMean: output mean of int;

count := 0;
s: stack of int;

visit(p, visitor {
	before node: CodeRepository -&gt; {
		# only look at the latest snapshot
		# and only include Java files
		snapshot := getsnapshot(node, "SOURCE_JAVA_JLS");
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	before node: Declaration -&gt; {
		# only interested in fields, which only occur inside (anonymous) classes
		if (node.kind == TypeKind.CLASS || node.kind == TypeKind.ANONYMOUS) {
			# store old value
			push(s, count);
			count = 0;

			# find uses and increment counter
			foreach (i: int; def(node.fields[i]))
				foreach (j: int; node.fields[i].modifiers[j].kind == ModifierKind.OTHER
								&& node.fields[i].modifiers[j].other == "transient")
					count++;
		} else
			stop;
	}
	after node: Declaration -&gt; {
		# output result
		TransientTotal &lt;&lt; count;
		TransientMax &lt;&lt; p.id weight count;
		TransientMin &lt;&lt; p.id weight count;
		TransientMean &lt;&lt; count;

		# restore previous value
		count = pop(s);
	}
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['npm'] = t('What are the number of public methods (NPM), per-project and per-type?');
$example_jobs['npm'] = 1598;
$examples['npm'] = <<<BOA
# Computes Number of Public Methods (NPM) for each project, per-type
# Output is: NPM[ProjectID][TypeName] = NPM value
p: Project = input;
NPM: output sum[string][string] of int;

visit(p, visitor {
	# only look at the latest snapshot
	before n: CodeRepository -&gt; {
		snapshot := getsnapshot(n);
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	before node: Declaration -&gt;
		if (node.kind == TypeKind.CLASS)
			foreach (i: int; has_modifier_public(node.methods[i]))
				NPM[p.id][node.name] &lt;&lt; 1;
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['noa'] = t('What are the number of attributes (NOA), per-project and per-type?');
$example_jobs['noa'] = 1599;
$examples['noa'] = <<<BOA
# Computes Number of Attributes (NOA) for each project, per-type
# Output is: NOA[ProjectID][TypeName] = NOA value
p: Project = input;
NOA: output sum[string][string] of int;

visit(p, visitor {
	# only look at the latest snapshot
	before n: CodeRepository -&gt; {
		snapshot := getsnapshot(n);
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	before node: Declaration -&gt;
		if (node.kind == TypeKind.CLASS)
			NOA[p.id][node.name] &lt;&lt; len(node.fields);
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['dominator'] = t('Dominator Analysis');
$example_jobs['dominator'] = 62915;
$examples['dominator'] = <<<BOA
op: output collection[string][string][string] of string;

type T = set of string;
allNodeIds: T;

dominators := traversal(node: CFGNode): T {
	doms: T;

	if (def(getvalue(node))) {
		doms = getvalue(node);
	} else {
		if (node.id == 0)
			add(doms, string(node.id));
		else
			doms = clone(allNodeIds);
	}

	foreach(i: int; def(getvalue(node.predecessors[i])))
		doms = intersect(doms, getvalue(node.predecessors[i]));
	add(doms, string(node.id));

	return doms;
};

fp := fixp(curr, prev: T): bool {
	return curr == prev;
};

doms: map[string] of T;

collect := traversal(node: CFGNode) {
	if (def(getvalue(node, dominators)))
		doms[string(node.id)] = getvalue(node, dominators);
};

visit(input, visitor {
	before node: CodeRepository -&gt; {
		snapshot := getsnapshot(node);
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	before method: Method -&gt; {
		cfg := getcfg(method);

		for (i := 0; i &lt; len(cfg.nodes); i++)
			add(allNodeIds, string(i));

		traverse(cfg, TraversalDirection.FORWARD, TraversalKind.REVERSEPOSTORDER, dominators, fp);
		traverse(cfg, TraversalDirection.FORWARD, TraversalKind.ITERATIVE, collect);

		op[input.project_url][current(ChangedFile).name][method.name] &lt;&lt; string(doms);

		clear(dominators);
		clear(collect);
		clear(doms);
		clear(allNodeIds);
	}
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['reaching'] = t('Reaching Definitions');
$example_jobs['reaching'] = 62922;
$examples['reaching'] = <<<BOA
m: output collection[string][int][int] of string;

# program analysis output
type T = set of string;
type T_gen_kill = { gen: string, kill: string };
type T_inout = { in: T, out: T };

m_name: string;

# traversal that accumulates generated values
cfg_def := traversal(node: CFGNode) : T_gen_kill {
	cur_val: T_gen_kill = { "", "" };
	if (node.defVariables != "") {
		cur_val.gen = node.defVariables + "@" + string(node.id);
		cur_val.kill = node.defVariables;
	}
	return cur_val;
};

# cfg reaching definition analysis
cfg_reach_def := traversal(n: CFGNode): T_inout {
	cur_val: T_inout;
	if (def(getvalue(n))) {
		cur_val = getvalue(n);
	} else {
		in_set: T;
		out_set: T;
		cur_val = { in_set, out_set };
	}

	preds := n.predecessors;
	foreach (i: int; def(preds[i])) {
		pred := getvalue(preds[i]);
		if (def(pred))
			cur_val.in = union(cur_val.in, pred.out);
	}

	cur_val.out = clone(cur_val.in);
	genkill := getvalue(n, cfg_def);

	if (genkill.kill != "") {
		tmp_out := values(cur_val.out);

		foreach (i: int; def(tmp_out[i])) {
			tmp1 := clone(tmp_out[i]);
			str_array := splitall(tmp1, "@");

			if (str_array[0] == genkill.kill)
				remove(cur_val.out, tmp1);
		}
		add(cur_val.out, genkill.gen);
	}
	return cur_val;
};

result := traversal(node: CFGNode) {
	if (def(getvalue(node, cfg_reach_def)))
		m[input.project_url][m_name][node.id] &lt;&lt; string(getvalue(node, cfg_reach_def).out);
};

# user-defined fix point function that is used for analysis termination.
fixp1 := fixp(curr, prev: T_inout) : bool {
	if (len(difference(curr.out, prev.out)) == 0)
		return true;
	return false;
};

visit(input, visitor {
	before node: CodeRepository -&gt; {
		snapshot := getsnapshot(node, "SOURCE_JAVA_JLS");
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	before node: Method -&gt; {
		cfg := getcfg(node);
		m_name = method.name;

		traverse(cfg, TraversalDirection.FORWARD, TraversalKind.HYBRID, cfg_def);
		traverse(cfg, TraversalDirection.FORWARD, TraversalKind.HYBRID, cfg_reach_def, fixp1);
		traverse(cfg, TraversalDirection.FORWARD, TraversalKind.HYBRID, result);

		clear(cfg_def);
		clear(cfg_reach_def);
	}
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['livevars'] = t('Live Variables');
$example_jobs['livevars'] = 62919;
$examples['livevars'] = <<<BOA
m: output collection[string][int][int] of string;

# program analysis output
type T = set of string;
type T_gen_kill = { gen: T, kill: string };
type T_inout = { in: T, out: T };

m_name: string;

# traversal that gets all variable uses in a method
init := traversal(node: CFGNode) : T_gen_kill {
	cur_value: T_gen_kill;
	cur_value = { node.useVariables, node.defVariables };
	return cur_value;
};

# cfg live variable analysis
live := traversal(node: CFGNode) : T_inout {
	cur_val: T_inout;
	if (def(getvalue(node))) {
		cur_val = getvalue(node);
	} else {
		in_set: T;
		out_set: T;
		cur_val = { in_set, out_set };
	}

	succs := node.successors;
	foreach(i:int; def(succs[i])) {
		succ := getvalue(succs[i]);
		if (def(succ)) {
			cur_val.out = union(cur_val.out,succ.in);
		}
	}
	gen_kill := getvalue(node, init);
	if (def(gen_kill)) {
		remove(cur_val.out, gen_kill.kill);
		cur_val.in = union(gen_kill.gen, cur_val.out);
	}
	return cur_val;
};

result := traversal(node: CFGNode) {
	if (def(getvalue(node, live))) {
		m[input.project_url][m_name][node.id] &lt;&lt; string(getvalue(node, live).in);
	}
};

# user-defined fix point function that is used for analysis termination.
fixp1 := fixp(curr, prev: T_inout) : bool {
	if (len(difference(curr.in, prev.in)) == 0)
		return true;
	return false;
};

visit(input, visitor {
	before node: CodeRepository -&gt; {
		snapshot := getsnapshot(node, "SOURCE_JAVA_JLS");
		foreach (i: int; def(snapshot[i]))
			visit(snapshot[i]);
		stop;
	}
	before node: Method -&gt; {
		cfg := getcfg(node);
		m_name = method.name;

		traverse(cfg, TraversalDirection.BACKWARD, TraversalKind.HYBRID, init);
		traverse(cfg, TraversalDirection.BACKWARD, TraversalKind.HYBRID, live, fixp1);
		traverse(cfg, TraversalDirection.BACKWARD, TraversalKind.HYBRID, result);

		clear(init);
		clear(live);
	}
});
BOA;

# ------------------------------------------------------------------------------------ #

$example_names['java-build'] = t('How often are popular Java build systems used?');
$example_jobs['java-build'] = 63197;
$examples['java-build'] = <<<BOA
# How often are popular Java build systems used?
TOTAL: output sum of int;

ANT: output sum of int;
GRADLE: output sum of int;
MAVEN: output sum of int;
MAKE: output sum of int;

NONE: output sum of int;

hasAnt := false;
hasGradle := false;
hasMvn := false;
hasMake := false;

exists (i: int; lowercase(input.programming_languages[i]) == "java")
	visit(input, visitor {
		before Project -&gt; TOTAL &lt;&lt; 1;
		after Project -&gt; {
			if (hasAnt) ANT &lt;&lt; 1;
			if (hasGradle) GRADLE &lt;&lt; 1;
			if (hasMvn) MAVEN &lt;&lt; 1;
			if (hasMake) MAKE &lt;&lt; 1;
			if (!(hasAnt || hasGradle || hasMvn || hasMake)) NONE &lt;&lt; 1;
		}
		before node: CodeRepository -&gt; {
			snapshot := getsnapshot(node);
			for (j := 0; j &lt; len(snapshot); j++) {
				if (match(`/build.xml$`, snapshot[j].name))
					hasAnt = true;
				else if (match(`/build.gradle$`, snapshot[j].name))
					hasGradle = true;
				else if (match(`/pom.xml$`, snapshot[j].name))
					hasMvn = true;
				else if (match(`/makefile$`, lowercase(snapshot[j].name)))
					hasMake = true;
			}
			stop;
		}
	});
BOA;

# ------------------------------------------------------------------------------------ #

$example_options = array(
	0 => t('-- Select Example --'),
	'Programming Languages' => array(
		'top-langs' => $example_names['top-langs'],
		'mult-langs' => $example_names['mult-langs'],
		'scheme-use' => $example_names['scheme-use'],
		//'java-added' => $example_names['java-added'],
	),
	'Project Management' => array(
		'proj-created' => $example_names['proj-created'],
		'proj-topics' => $example_names['proj-topics'],
		'active-proj' => $example_names['active-proj'],
		'svn-added' => $example_names['svn-added'],
		'num-revisions' => $example_names['num-revisions'],
		'fix-bugs' => $example_names['fix-bugs'],
		'committers-proj' => $example_names['committers-proj'],
		//'proj-committers' => $example_names['proj-committers'],
		'churn-rate' => $example_names['churn-rate'],
		'commits-years' => $example_names['commits-years'],
		//'word-dist' => $example_names['word-dist'],
		'java-build' => $example_names['java-build'],
	),
	'Legal' => array(
		'top-license' => $example_names['top-license'],
		'mult-license' => $example_names['mult-license'],
	),
	'Platform/Environment' => array(
		'top-os' => $example_names['top-os'],
		'mult-os' => $example_names['mult-os'],
		'top-db' => $example_names['top-db'],
		'mult-db' => $example_names['mult-db'],
		'db-pl' => $example_names['db-pl'],
	),
	'Source Code' => array(
		'ast-count' => $example_names['ast-count'],
		'java-count' => $example_names['java-count'],
		'null-check' => $example_names['null-check'],
		'dead-code' => $example_names['dead-code'],
		'gen-fields' => $example_names['gen-fields'],
		'varargs' => $example_names['varargs'],
		'transient' => $example_names['transient'],
	),
	'Software Engineering Metrics' => array(
		'noa' => $example_names['noa'],
		'npm' => $example_names['npm'],
	),
	'Program Analysis' => array(
		'dominator' => $example_names['dominator'],
		'livevars' => $example_names['livevars'],
		'reaching' => $example_names['reaching'],
	),
);

?>
