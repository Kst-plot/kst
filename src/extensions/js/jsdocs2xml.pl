#!/usr/bin/perl
#


open(INPUT, "$ARGV[0]") || die "Can't open input file $ARGV[0]: $!";

$inComment = 0;
$thisComment = "";

while (<INPUT>) {
	$this = $_;
	while ($this) {
		$_ = $this;
		if ($inComment) {
			if (/(.*?)\*\/(.*)/) {
				$this = $2;
				$inComment = 0;
				@comments[$#comments + 1] = $thisComment . $1;
				$thisComment = "";
			} else {
				$thisComment .= $this;
				$this = "";
			}
		} else {
			if (/(.*?)\/\*(.*)/) { #comment
				$_ = $2;
				if (/(.*?)\*\/(.*)/) {
					# Comment ends on this line too.
					$this = $2;
					@comments[$#comments + 1] = $1;
				} else { # Comment continues past this line
					$inComment = 1;
					$thisComment = $_;
					$this = "";
				}
			} else { # Code
				$this = "";
			}
		}
	}
}

$className = "";

foreach (@comments) {
	if (/\s+\@class\s*(\w+)/) {
		$className = $1;
		break;
	}
}


sub escape($) {
	$rc = @_[0];
	$rc =~ s/([^\'\"\/\\\(\)\[\]\{\}\.\,\w\s])/sprintf ("&#%d;", ord ($1))/ges;
	return $rc;
}


sub extractClassComment(*$;$) {
	$text = @_[1];
	$indent = @_[2];
	$handle = @_[0];
	while ($text) {
		$_ = $text;
		if (/(.*?)\@(\w+)\s+(.*)/s) {
			$rest = $3;
			$_ = $2;
			if (/^class$/) {
				# do nothing
			} elsif (/^obsolete$/) {
				print $handle "$indent  <obsolete/>\n";
			} elsif (/^description$/) {
				$_ = $rest;
				if (/(.*?)(\@\w+.*|$)/s) {
					$dt = escape($1);
					print $handle "$indent<description>\n";
					print $handle "$indent$dt\n";
					print $handle "$indent</description>\n";
					$rest = $2;
				} else {
					print "Error: \@description requires an argument\n";
					return;
				}
			} elsif (/^inherits$/) {
				$_ = $rest;
				if (/(\w+)(.*)/s) {
					print $handle "$indent<inherits name=\"$1\"/>\n";
					$rest = $2;
				} else {
					print "Error: \@inherits requires one argument\n";
					return;
				}
			} elsif (/^collection$/) {
# FIXME: two args means the second one is the "global" collection
				$_ = $rest;
				if (/(\w+)(.*)/s) {
					print $handle "$indent<collection name=\"$1\"/>\n";
					$rest = $2;
				} else {
					print "Error: \@collection requires at least one argument\n";
					return;
				}
			} else {
				print "Found an unknown comment type: $_\n";
			}

			$text = $rest;
		} else {
			return;
		}
	}
}


sub extractMethod(*$$$;$) {
	$text = @_[1];
	$type = @_[2];
	$name = @_[3];
	$indent = @_[4];
	$handle = @_[0];
	$return = "";
	$_ = $text;
	if (/(.*?)\@returns\s+([\w\[\]]+)/) {
		$return = "type=\"$2\"";
	}
	$obsolete = "";
	if (/\@obsolete/) {
		$obsolete = "obsolete=\"true\"";
	}
	print $handle "$indent<$type name=\"$name\" $return $obsolete>\n";
	while ($text) {
		$_ = $text;
		if (/(.*?)\@(\w+)\s+(.*)/s) {
			$rest = $3;
			$_ = $2;
			if (/^$type$/) {
				# do nothing
			} elsif (/^returns$/) {
				# do nothing
			} elsif (/^obsolete$/) {
				# do nothing
			} elsif (/^description$/) {
				$_ = $rest;
				if (/(.*?)(\@\w+.*|$)/s) {
					$dt = escape($1);
					print $handle "$indent  <description>\n";
					print $handle "$indent  $dt\n";
					print $handle "$indent  </description>\n";
					$rest = $2;
				} else {
					print "Error: \@description requires an argument\n";
					return;
				}
			} elsif (/^arg$/ or /^optarg$/) {
				$opt = "";
				if (/opt/) {
					$opt = " optional=\"true\"";
				}
				$_ = $rest;
				if (/\s*([\w\[\]]+)\s*(\w+)\s*(.*?)(\@\w+.*|$)/s) {
					$atype = $1;
					$aname = $2;
					$adesc = escape($3);
					$rest = $4;
					print $handle "$indent  <argument name=\"$aname\" type=\"$atype\"$opt>\n";
					print $handle "$indent    <description>\n";
					print $handle "$indent    $adesc\n";
					print $handle "$indent    </description>\n";
					print $handle "$indent  </argument>\n";
				} else {
					print "Error: \@arg requires three arguments\n";
					return;
				}
			} elsif (/^exception$/) {
				$_ = $rest;
				if (/\s*(\w+)\s*(.*?)(\@\w+.*|$)/s) {
					$ename = $1;
					$edesc = escape($2);
					$rest = $3;
					print $handle "$indent  <exception name=\"$ename\">\n";
					print $handle "$indent    <description>\n";
					print $handle "$indent    $edesc\n";
					print $handle "$indent    </description>\n";
					print $handle "$indent  </exception>\n";
				} else {
					print "Error: \@exception requires two arguments\n";
					return;
				}
			} else {
				print "Found an unknown comment type: $_\n";
			}

			$text = $rest;
		} else {
			$text = "";
		}
	}
	print $handle "$indent</$type>\n";
}


sub extractProperty(*$$$;$) {
	$text = @_[1];
	$type = @_[2];
	$name = @_[3];
	$indent = @_[4];
	$handle = @_[0];
	$_ = $text;
	$readonly = "";
	if (/\@readonly/) {
		$readonly = "readonly=\"true\"";
	}
	$obsolete = "";
	if (/\@obsolete/) {
		$obsolete = "obsolete=\"true\"";
	}
	print $handle "$indent<property name=\"$name\" type=\"$type\" $readonly $obsolete>\n";
	while ($text) {
		$_ = $text;
		if (/(.*?)\@(\w+)\s+(.*)/s) {
			$rest = $3;
			$_ = $2;
			if (/^property$/) {
				# do nothing
			} elsif (/^readonly$/) {
				# do nothing
			} elsif (/^obsolete$/) {
				# do nothing
			} elsif (/^description$/) {
				$_ = $rest;
				if (/(.*?)(\@\w+.*|$)/s) {
					$dt = escape($1);
					print $handle "$indent  <description>\n";
					print $handle "$indent  $dt\n";
					print $handle "$indent  </description>\n";
					$rest = $2;
				} else {
					print "Error: \@description requires an argument\n";
					return;
				}
			} else {
				print "Found an unknown comment type: $_\n";
			}

			$text = $rest;
		} else {
			$text = "";
		}
	}
	print $handle "$indent</property>\n";
}


sub extractAndDump(*$) {
	$handle = @_[0];
	$text = @_[1];
	if (/(.*?)\@(\w+)\s+(\w+)?/s) {
		$_ = $2;
		$name = $3;
		if (/^class$/) {
			extractClassComment($handle, $text, "  ");
		} elsif (/^constructor$/) {
			extractMethod($handle, $text, "constructor", $name, "  ");
		} elsif (/^method$/) {
			extractMethod($handle, $text, "method", $name, "  ");
		} elsif (/^property$/) {
			$_ = $text;
			if (/(.*?)\@(\w+)\s+([\w\[\]]+)\s+(\w+)/) {
				extractProperty($handle, $text, $3, $4, "  ");
			} else {
				print "Error: \@property requires two arguments\n";
			}
		} else {
			print "Found an unknown comment type: $_\n";
		}
	}
}

use File::Basename;

if ($className ne "") { # found a class
	print "Class: $className\n";
	$fn = $ARGV[0];
	$fn =~ s/^(.*\.)(.*?)$/$1xml/;
	$fn = basename($fn);
	open(OUTPUT, ">$fn") || die "Can't open output file $fn: $!";
	print OUTPUT "<class name=\"$className\">\n";
	foreach (@comments) {
		# Extract each @(\w+) and parse if we understand it
		extractAndDump(\*OUTPUT, $_);
	}
	print OUTPUT "</class>\n";
}

