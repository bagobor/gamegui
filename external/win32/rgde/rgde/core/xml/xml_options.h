///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.3
// --------------------------------------------------------
// Copyright (C) 2006-2007, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
// This work is based on the pugxml parser, which is:
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

/// XML Parser namespace.
namespace xml
{
	// Parsing options

	/**
	* Memory block size, used for fast allocator. Memory for DOM tree is allocated in blocks of
	* memory_block_size + 4.
	* This value affects size of xml_memory class.
	*/
	const size_t memory_block_size = 32768;

	/**
	* Minimal parsing mode. Equivalent to turning all other flags off. This set of flags means
	* that pugixml does not add pi/cdata sections or comments to DOM tree and does not perform
	* any conversions for input data, meaning fastest parsing.
	*/
	const unsigned int parse_minimal			= 0x0000;

	/**
	* This flag determines if processing instructions (nodes with type node_pi; such nodes have the
	* form of <? target content ?> or <? target ?> in XML) are to be put in DOM tree. If this flag is off,
	* they are not put in the tree, but are still parsed and checked for correctness.
	*
	* The corresponding node in DOM tree will have type node_pi, name "target" and value "content",
	* if any.
	*
	* Note that <?xml ...?> (document declaration) is not considered to be a PI.
	*
	* This flag is off by default.
	*/
	const unsigned int parse_pi					= 0x0001;

	/**
	* This flag determines if comments (nodes with type node_comment; such nodes have the form of
	* <!-- content --> in XML) are to be put in DOM tree. If this flag is off, they are not put in
	* the tree, but are still parsed and checked for correctness.
	*
	* The corresponding node in DOM tree will have type node_comment, empty name and value "content".
	*
	* This flag is off by default.
	*/
	const unsigned int parse_comments			= 0x0002;

	/**
	* This flag determines if CDATA sections (nodes with type node_cdata; such nodes have the form
	* of <![CDATA[[content]]> in XML) are to be put in DOM tree. If this flag is off, they are not
	* put in the tree, but are still parsed and checked for correctness.
	*
	* The corresponding node in DOM tree will have type node_cdata, empty name and value "content".
	*
	* This flag is on by default.
	*/
	const unsigned int parse_cdata				= 0x0004;

	/**
	* This flag determines if nodes with PCDATA (regular text) that consist only of whitespace
	* characters are to be put in DOM tree. Often whitespace-only data is not significant for the
	* application, and the cost of allocating and storing such nodes (both memory and speed-wise)
	* can be significant. For example, after parsing XML string "<node> <a/> </node>", <node> element
	* will have 3 children when parse_ws_pcdata is set (child with type node_pcdata and value=" ",
	* child with type node_element and name "a", and another child with type node_pcdata and
	* value=" "), and only 1 child when parse_ws_pcdata is not set.
	* 
	* This flag is off by default.
	*/
	const unsigned int parse_ws_pcdata			= 0x0008;

	/**
	* This flag determines if character and entity references are to be expanded during the parsing
	* process. Character references are &amp;#...; or &amp;#x...; (... is Unicode numeric representation of
	* character in either decimal (&amp;#...;) or hexadecimal (&amp;#x...;) form), entity references are &amp;...;
	* Note that as pugixml does not handle DTD, the only allowed entities are predefined ones - 
	* &amp;lt;, &amp;gt;, &amp;amp;, &amp;apos; and &amp;quot;. If character/entity reference can not be expanded, it is
	* leaved as is, so you can do additional processing later.
	* Reference expansion is performed in attribute values and PCDATA content.
	*
	* This flag is on by default.
	*/
	const unsigned int parse_escapes			= 0x0010;

	/**
	* This flag determines if EOL handling (that is, replacing sequences 0x0d 0x0a by a single 0x0a
	* character, and replacing all standalone 0x0d characters by 0x0a) is to be performed on input
	* data (that is, comments contents, PCDATA/CDATA contents and attribute values).
	*
	* This flag is on by default.
	*/
	const unsigned int parse_eol				= 0x0020;

	/**
	* This flag determines if attribute value normalization should be performed for all attributes,
	* assuming that their type is not CDATA. This means, that:
	* 1. Whitespace characters (new line, tab and space) are replaced with space (' ')
	* 2. Afterwards sequences of spaces are replaced with a single space
	* 3. Leading/trailing whitespace characters are trimmed
	* 
	* This flag is off by default.
	*/
	const unsigned int parse_wnorm_attribute	= 0x0040;

	/**
	* This flag determines if attribute value normalization should be performed for all attributes,
	* assuming that their type is CDATA. This means, that whitespace characters (new line, tab and
	* space) are replaced with space (' '). Note, that the actions performed while this flag is on
	* are also performed if parse_wnorm_attribute is on, so this flag has no effect if
	* parse_wnorm_attribute flag is set.
	* 
	* This flag is on by default.
	*/
	const unsigned int parse_wconv_attribute	= 0x0080;

	/**
	* This is the default set of flags. It includes parsing CDATA sections (comments/PIs are not
	* parsed), performing character and entity reference expansion, replacing whitespace characters
	* with spaces in attribute values and performing EOL handling. Note, that PCDATA sections
	* consisting only of whitespace characters are not parsed (by default) for performance reasons.
	*/
	const unsigned int parse_default			= parse_cdata | parse_escapes | parse_wconv_attribute | parse_eol;

	// Formatting flags

	/**
	* Indent the nodes that are written to output stream with as many indentation strings as deep
	* the node is in DOM tree.
	*
	* This flag is on by default.
	*/
	const unsigned int format_indent	= 0x01;

	/**
	* This flag determines how the non-printable symbols are written to output stream - they are
	* either considered UTF-8 and are written as UTF-8 character, escaped with &#...;, or they are
	* considered to be ASCII and each ASCII character is escaped separately.
	*
	* This flag is on by default.
	*/
	const unsigned int format_utf8		= 0x02;

	/**
	* This flag determines if UTF-8 BOM is to be written to output stream.
	*
	* This flag is off by default.
	*/
	const unsigned int format_write_bom	= 0x04;

	/**
	* If this flag is on, no indentation is performed and no line breaks are written to output file.
	* This means that the data is written to output stream as is.
	*
	* This flag is off by default.
	*/
	const unsigned int format_raw		= 0x08;

	/**
	* This is the default set of formatting flags. It includes indenting nodes depending on their
	* depth in DOM tree and considering input data to be UTF-8.
	*/
	const unsigned int format_default	= format_indent | format_utf8;
}