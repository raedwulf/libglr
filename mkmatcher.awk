#!/usr/bin/awk -f
#
# Copyright (C) 2018 Tai Chi Minh Ralph Eastwood <tcmreastwood@gmail.com> 
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# simple implementation of quick sort
function qsortr(A, left, right,   i, last) {
	if (left >= right)
		return
	swap(A, left, left+int((right-left+1)*rand()))
	last = left
	for (i = left+1; i <= right; i++) {
		if (A[i] > A[left])
			swap(A, ++last, i)
	}
	swap(A, left, last)
	qsortr(A, left, last-1)
	qsortr(A, last+1, right)
}

function swap(A, i, j,   t) {
	t = A[i]; A[i] = A[j]; A[j] = t
}

# matches string a and b and returns length of prefix matched
function matchprefix(a,b,    l,la,lb,i,x,y,n) {
	la = length(a)
	lb = length(b)
	if (la < lb) {
		l = la
	} else {
		l = lb
	}
	n = 0
	for (i = 1; i <= l; i++) {
		x = substr(a,i,1)
		y = substr(b,i,1)
		if (x == y) {
			n++
		} else {
			break
		}
	}
	return n
}

# insert entry into radix tree
function radixinsert(r,n,k,v,    cs,m,i,op,tp,np,cp) {
	cs = split(node[r "_" n], ca, " ")
	m = 0
	# check each child
	for (i in ca) {
		op = prefix[r "_" ca[i]]
		m = matchprefix(k,op)
		# check matched entire node
		if (m == length(op)) { # if yes, traverse child
			cp = substr(k,m+1,length(k)-m)
			# if it matches this node and only this node,
			# then we can just mark the current node for
			# the desired value because the key points to this node
			if (cp == "") {
				value[r "_" ca[i]] = v
			} else {
				radixinsert(r,ca[i],cp,v)
			}
			return
		} else if (m > 0) { # split node at i
			tp = substr(op,m+1,length(op)-m) # split prefix
			np = substr(k,m+1,length(k)-m)   # new prefix
			#print "oldprefix:" op
			#print "splitdata: " tp
			#print "newdata: " np
			#print "newparent:" substr(op,1,m)
			# figure out if we need to add one or two nodes
			# if the old node needs to be split, then we get two nodes
			# 1 new node gets next part of prefix, and inherits old node's children
			if (tp != "") {
				node[r "_" nodecount[r]] = node[r "_" ca[i]]
				#print "inherit: " value[r "_" ca[i]] > "/dev/stderr"
				prefix[r "_" nodecount[r]] = tp
				value[r "_" nodecount[r]] = value[r "_" ca[i]] # copy old value
				node[r "_" ca[i]] = "" nodecount[r] # set the old node to point to this child
				nodecount[r]++
			}
			# 1 new node contains the new data
			if (np != "") {
				node[r "_" nodecount[r]] = ""
				#print "newdata: " np " - "  v > "/dev/stderr"
				prefix[r "_" nodecount[r]] = np
				value[r "_" nodecount[r]] = v
				# old node has original part of prefix, and has 1-2 new nodes as children
				node[r "_" ca[i]] = node[r "_" ca[i]] " " nodecount[r]
				nodecount[r]++
			}
			# split the original prefix to the first part
			prefix[r "_" ca[i]] = substr(op,1,m)
			if (tp != "") {
				value[r "_" ca[i]] = "" # old value means nothing if placed in one of the new nodes
			}
			return
		}
	}
	# if there were no matching children, create one in current node
	if (m == 0) {
		if (node[r "_" n] != "") {
			node[r "_" n] = node[r "_" n] " " nodecount[r]
		} else {
			node[r "_" n] = "" nodecount[r]
		}
		#print "nomatching: " k
		node[r "_" nodecount[r]] = ""
		prefix[r "_" nodecount[r]] = k
		value[r "_" nodecount[r]] = v
		nodecount[r]++
	}
}

# hex functions
function hex(n) {
	return sprintf("%02x", n)
}

function hexstr(s,l,    i,t) {
	h = ""
	for (i = 0; i < l; i++) {
		t = substr(s, i + 1, 1)
		h = hex(ord[t]) h
	}
	if (l > 4)
		return "0x" h "ull"
	else
		return "0x" h "u"
}

function s2t(b) {
	return "const uint" (8 * b) "_t"
}

# generate conditional expressions
function condexpr_(s, i, b, m,    ph, n) {
	p = substr(s, i + 1, b)
	ph = hexstr(p, b)
	n = "*(" s2t(b) " *)&p[" i "] == " ph
	if (length(m) > 0) {
		m = m " && " n 
	} else {
		m = n
	}
	return m
}

function condexpr(s,l,   i,m) {
	i = 0
	for (;i <= l-8;i += 8) {
		m = condexpr_(s, i, 8, m)
	}
	if (l - i >= 4) {
		m = condexpr_(s, i, 4, m)
		i+=4
	}
	if (l - i >= 2) {
		m = condexpr_(s, i, 2, m)
		i+=2
	}
	if (l - i >= 1) m = condexpr_(s, i, 1, m)
	return m
}

function ifexpr(e, d, b) {
	print d "if (" e ") " (b ? "{" : "") " /* " "'" p "'" " */"
}

# dump the nodes and the edge links straight into code
function codegen(r,d,    i,c,cs,ca,e,t,l,sr,nc,p) {
	nc = nodecount[r]
	for (i = 0; i < nc; i++) {
		sr = 0
		cs = split(node[r "_" i], ca, " ")
		if (i != 0 && node[r "_" i] != "") {
			print label r "_" i ":"
		}
		for (c in ca) {
			p = prefix[r "_" ca[c]]
			if (p == "" || node[r "_" ca[c]] == "") {
				t = 1
			} else {
				t = 0
			}
			l = length(p)
			e = condexpr(p, l + ((null == 1) ? t : 0)) # add 1 to length if terminal for null char
			needif = (null == 1 || p != "")
			# check if target node is leaf
			if (node[r "_" ca[c]] == "") {
				# generate early return; TODO: two lines the same... indent is different
				if (needif) {
					ifexpr(e, d, 0)
					print d "\treturn " value[r "_" ca[c]] ";"
				} else {
					print d "return " value[r "_" ca[c]] ";"
					sr = 1 # skip final return
				}
			} else {
				# generate branch
				if (needif) ifexpr(e, d, 1)
				print d "\tp += " l "; goto " label r "_" ca[c] ";"
				if (needif) print d "}"
			}
		}
		if (node[r "_" i] != "" && sr == 0) {
			print d "return -1;"
		}
	}
}

#function dump(a,n,    k) {
#	for (k in a) {
#		print n "[" k "] = " a[k] > "/dev/stderr"
#	}
#}

BEGIN {
	FS=";"

	# ascii conversion table
	for(i = 0; i < 256; i++) ord[sprintf("%c",i)] = i

	if (!fn) fn = "match"
	if (!label) label = "l"
	if (null == "") {
		null = 1
	}
	if (!headers) headers = ""
	size=0

	hs = split(headers, ha, ";")
	print "/* See LICENSE file for copyright and license details."
	print " * Generated by mkmatcher.awk"
	print " */"
	print
	print "#include <stddef.h>"
	print "#include <stdint.h>"
	if (hs > 0) {
		print
		print "/* headers included using headers= parameter */"
	}
	for (h in ha) {
		print "#include " ha[h]
	}
	print
	print "int"
	print fn "(const char *s, size_t l)"
	print "{"
	print "\tconst char *p = s;"
}

{
	# build the radix tree while reading input
	key=$1
	val=$2
	l = length(key)
	# create a new tree if necessary for a new length
	if (!(nodecount[l] > 0)) {
		if (lens == "")
			lens = length(key)
		else
			lens = lens " " length(key)
		node[l "_" 0] = ""
		prefix[l "_" 0] = ""
		value[l "_" 0] = ""
		nodecount[l] = 1
		if (l > size) size = l
	}
	# insert into tree for length l
	radixinsert(l,0,key,val)
}

END {
	#dump(node, "node")
	#dump(prefix, "prefix")
	#dump(value, "value")
	ls = split(lens, la, " ")
	qsortr(la, 1, ls)

	print "\tswitch (l) {"
	for (n in la) {
		print "\tcase " (la[n] + null) ":"
		codegen(la[n],"\t\t")
	}
	print "\tdefault: return -1;"
	print "\t}"
	print "}"
}
