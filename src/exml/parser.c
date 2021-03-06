/*
 *  Copyright (C) 2002-2003 the xine project
 *
 *  This file is part of xine, a free video player.
 *  This file is part of gxine, a free video player.
 *
 *  xine is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  xine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 *  $Id: xmlparser.c,v 1.15 2006/02/14 02:25:01 dsalt Exp $
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define LOG_MODULE "exmlparser"
#define LOG_VERBOSE
/*
#define LOG
*/

/*#include "xineutils.h"*/
#include "exml/lexer.h"
#include "exml/parser.h"


#define TOKEN_SIZE  4 * 1024
#define DATA_SIZE   4 * 1024
#define MAX_RECURSION 10

/* private global variables */
static int xml_parser_mode;

/* private functions */

static char * strtoupper(char * str) {
  int i = 0;

  while (str[i] != '\0') {
    str[i] = (char)toupper((int)str[i]);
    i++;
  }
  return str;
}

static exml_node_t * new_exml_node(void) {
  exml_node_t * new_node;

  new_node = (exml_node_t*) malloc(sizeof(exml_node_t));
  new_node->name  = NULL;
  new_node->data  = NULL;
  new_node->props = NULL;
  new_node->child = NULL;
  new_node->next  = NULL;
  return new_node;
}

static void free_exml_node(exml_node_t * node) {
  free (node->name);
  free (node->data);
  free(node);
}

static exml_property_t * new_exml_property(void) {
  exml_property_t * new_property;

  new_property = (exml_property_t*) malloc(sizeof(exml_property_t));
  new_property->name  = NULL;
  new_property->value = NULL;
  new_property->next  = NULL;
  return new_property;
}

static void free_exml_property(exml_property_t * property) {
  free (property->name);
  free (property->value);
  free(property);
}

void exml_parser_init(const char * buf, int size, int mode) {

  elexer_init(buf, size);
  xml_parser_mode = mode;
}

static void exml_parser_free_props(exml_property_t *current_property) {
  if (current_property) {
    if (!current_property->next) {
      free_exml_property(current_property);
    } else {
      exml_parser_free_props(current_property->next);
      free_exml_property(current_property);
    }
  }
}

static void exml_parser_free_tree_rec(exml_node_t *current_node, int free_next) {
  /*lprintf("xml_parser_free_tree_rec: %s\n", current_node->name);*/

  if (current_node) {
    /* properties */
    if (current_node->props) {
      exml_parser_free_props(current_node->props);
    }

    /* child nodes */
    if (current_node->child) {
      /*lprintf("xml_parser_free_tree_rec: child\n");*/
      exml_parser_free_tree_rec(current_node->child, 1);
    }

    /* next nodes */
    if (free_next) {
      exml_node_t *next_node = current_node->next;
      exml_node_t *next_next_node;

      while (next_node) {
        next_next_node = next_node->next;
        /*lprintf("xml_parser_free_tree_rec: next\n");*/
        exml_parser_free_tree_rec(next_node, 0);
        next_node = next_next_node;
      }
    }

    free_exml_node(current_node);
  }
}

void exml_parser_free_tree(exml_node_t *current_node) {
  /*lprintf("xml_parser_free_tree\n");*/
   exml_parser_free_tree_rec(current_node, 1);
}

#define STATE_IDLE    0
#define STATE_NODE    1
#define STATE_COMMENT 7

static int exml_parser_get_node (exml_node_t *current_node, char *root_name, int rec) {
  char tok[TOKEN_SIZE];
  char property_name[TOKEN_SIZE];
  char node_name[TOKEN_SIZE];
  int state = STATE_IDLE;
  int res = 0;
  int parse_res;
  int bypass_get_token = 0;
  exml_node_t *subtree = NULL;
  exml_node_t *current_subtree = NULL;
  exml_property_t *current_property = NULL;
  exml_property_t *properties = NULL;

  if (rec < MAX_RECURSION) {

    while ((bypass_get_token) || (res = elexer_get_token(tok, TOKEN_SIZE)) != T_ERROR) {
      bypass_get_token = 0;
      /*lprintf("info: %d - %d : '%s'\n", state, res, tok);*/

      switch (state) {
      case STATE_IDLE:
	switch (res) {
	case (T_EOL):
	case (T_SEPAR):
	  /* do nothing */
	  break;
	case (T_EOF):
	  return 0; /* normal end */
	  break;
	case (T_M_START_1):
	  state = STATE_NODE;
	  break;
	case (T_M_START_2):
	  state = 3;
	  break;
	case (T_C_START):
	  state = STATE_COMMENT;
	  break;
	case (T_TI_START):
	  state = 8;
	  break;
	case (T_DOCTYPE_START):
	  state = 9;
	  break;
	case (T_DATA):
	  /* current data */
	  if (current_node->data) {
	    /* avoid a memory leak */
	    free(current_node->data);
	  }
	  current_node->data = elexer_decode_entities(tok);
	  /*lprintf("info: node data : %s\n", current_node->data);*/
	  break;
	default:
	  /*lprintf("error: unexpected token \"%s\", state %d\n", tok, state);*/
	  return -1;
	  break;
	}
	break;

      case STATE_NODE:
	switch (res) {
	case (T_IDENT):
	  properties = NULL;
	  current_property = NULL;

	  /* save node name */
	  if (xml_parser_mode == XML_PARSER_CASE_INSENSITIVE) {
	    strtoupper(tok);
	  }
	  strcpy(node_name, tok);
	  state = 2;
	  /*lprintf("info: current node name \"%s\"\n", node_name);*/
	  break;
	default:
	  /*lprintf("error: unexpected token \"%s\", state %d\n", tok, state);*/
	  return -1;
	  break;
	}
	break;
      case 2:
	switch (res) {
	case (T_EOL):
	case (T_SEPAR):
	  /* nothing */
	  break;
	case (T_M_STOP_1):
	  /* new subtree */
	  subtree = new_exml_node();

	  /* set node name */
	  subtree->name = strdup(node_name);

	  /* set node propertys */
	  subtree->props = properties;
	  /*lprintf("info: rec %d new subtree %s\n", rec, node_name);*/
	  parse_res = exml_parser_get_node(subtree, node_name, rec + 1);
	  if (parse_res != 0) {
	    return parse_res;
	  }
	  if (current_subtree == NULL) {
	    current_node->child = subtree;
	    current_subtree = subtree;
	  } else {
	    current_subtree->next = subtree;
	    current_subtree = subtree;
	  }
	  state = STATE_IDLE;
	  break;
	case (T_M_STOP_2):
	  /* new leaf */
	  /* new subtree */
	  subtree = new_exml_node();

	  /* set node name */
	  subtree->name = strdup (node_name);

	  /* set node propertys */
	  subtree->props = properties;

	  /*lprintf("info: rec %d new subtree %s\n", rec, node_name);*/

	  if (current_subtree == NULL) {
	    current_node->child = subtree;
	    current_subtree = subtree;
	  } else {
	    current_subtree->next = subtree;
	    current_subtree = subtree; 
	  }
	  state = STATE_IDLE;
	  break;
	case (T_IDENT):
	  /* save property name */
	  if (xml_parser_mode == XML_PARSER_CASE_INSENSITIVE) {
	    strtoupper(tok);
	  }
	  strcpy(property_name, tok);
	  state = 5;
	  /*lprintf("info: current property name \"%s\"\n", property_name);*/
	  break;
	default:
	  /*lprintf("error: unexpected token \"%s\", state %d\n", tok, state);*/
	  return -1;
	  break;
	}
	break;

      case 3:
	switch (res) {
	case (T_IDENT):
	  /* must be equal to root_name */
	  if (xml_parser_mode == XML_PARSER_CASE_INSENSITIVE) {
	    strtoupper(tok);
	  }
	  if (strcmp(tok, root_name) == 0) {
	    state = 4;
	  } else {
	    /*lprintf("error: xml struct, tok=%s, waited_tok=%s\n", tok, root_name);*/
	    return -1;
	  }
	  break;
	default:
	  /*lprintf("error: unexpected token \"%s\", state %d\n", tok, state);*/
	  return -1;
	  break;
	}
	break;

				/* > expected */
      case 4:
	switch (res) {
	case (T_M_STOP_1):
	  return 0;
	  break;
	default:
	  /*lprintf("error: unexpected token \"%s\", state %d\n", tok, state);*/
	  return -1;
	  break;
	}
	break;

				/* = or > or ident or separator expected */
      case 5:
	switch (res) {
	case (T_EOL):
	case (T_SEPAR):
	  /* do nothing */
	  break;
	case (T_EQUAL):
	  state = 6;
	  break;
	case (T_IDENT):
	  bypass_get_token = 1; /* jump to state 2 without get a new token */
	  state = 2;
	  break;
	case (T_M_STOP_1):
	  /* add a new property without value */
	  if (current_property == NULL) {
	    properties = new_exml_property();
	    current_property = properties;
	  } else {
	    current_property->next = new_exml_property();
	    current_property = current_property->next;
	  }
	  current_property->name = strdup (property_name);
	  /*lprintf("info: new property %s\n", current_property->name);*/
	  bypass_get_token = 1; /* jump to state 2 without get a new token */
	  state = 2;
	  break;
	default:
	  /*lprintf("error: unexpected token \"%s\", state %d\n", tok, state);*/
	  return -1;
	  break;
	}
	break;

				/* string or ident or separator expected */
      case 6:
	switch (res) {
	case (T_EOL):
	case (T_SEPAR):
	  /* do nothing */
	  break;
	case (T_STRING):
	case (T_IDENT):
	  /* add a new property */
	  if (current_property == NULL) {
	    properties = new_exml_property();
	    current_property = properties;
	  } else {
	    current_property->next = new_exml_property();
	    current_property = current_property->next;
	  }
	  current_property->name = strdup(property_name);
	  current_property->value = elexer_decode_entities(tok);
	  /*lprintf("info: new property %s=%s\n", current_property->name, current_property->value);*/
	  state = 2;
	  break;
	default:
	  /*lprintf("error: unexpected token \"%s\", state %d\n", tok, state);*/
	  return -1;
	  break;
	}
	break;

				/* --> expected */
      case STATE_COMMENT:
	switch (res) {
	case (T_C_STOP):
	  state = STATE_IDLE;
	  break;
	default:
	  state = STATE_COMMENT;
	  break;
	}
	break;

				/* > expected */
      case 8:
	switch (res) {
	case (T_TI_STOP):
	  state = 0;
	  break;
	default:
	  state = 8;
	  break;
	}
	break;

				/* ?> expected */
      case 9:
	switch (res) {
	case (T_M_STOP_1):
	  state = 0;
	  break;
	default:
	  state = 9;
	  break;
	}
	break;


      default:
	/*lprintf("error: unknown parser state, state=%d\n", state);*/
	return -1;
      }
    }
    /* lex error */
    /*lprintf("error: lexer error\n");*/
    return -1;
  } else {
    /* max recursion */
    /*lprintf("error: max recursion\n");*/
    return -1;
  }
}

int exml_parser_build_tree(exml_node_t **root_node) {
  exml_node_t *tmp_node;
  int res;

  tmp_node = new_exml_node();
  res = exml_parser_get_node(tmp_node, "", 0);
  if ((tmp_node->child) && (!tmp_node->child->next)) {
    *root_node = tmp_node->child;
    free_exml_node(tmp_node);
    res = 0;
  } else {
    /*lprintf("error: xml struct\n");*/
    exml_parser_free_tree(tmp_node);
    res = -1;
  }
  return res;
}

char *exml_parser_get_property (const exml_node_t *node, const char *name) {

  exml_property_t *prop;

  prop = node->props;
  while (prop) {

    /*lprintf ("looking for %s in %s\n", name, prop->name);*/

    if (!strcasecmp (prop->name, name)) {
      /*lprintf ("found it. value=%s\n", prop->value);*/
      return prop->value;
    }

    prop = prop->next;
  }

  return NULL;
}

int exml_parser_get_property_int (const exml_node_t *node, const char *name, 
				 int def_value) {

  char *v;
  int   ret;

  v = exml_parser_get_property (node, name);

  if (!v)
    return def_value;

  if (sscanf (v, "%d", &ret) != 1)
    return def_value;
  else
    return ret;
}

int exml_parser_get_property_bool (const exml_node_t *node, const char *name, 
				  int def_value) {

  char *v;

  v = exml_parser_get_property (node, name);

  if (!v)
    return def_value;

  return !strcasecmp (v, "true");
}

static int exml_escape_string_internal (char *buf, const char *s,
				       exml_escape_quote_t quote_type)
{
  int c, length = 0;
  int sl = buf ? 8 : 0;
  /* calculate max required buffer size */
  while ((c = *s++ & 0xFF))
    switch (c)
    {
    case '"':  if (quote_type != XML_ESCAPE_DOUBLE_QUOTE) goto literal;
	       length += snprintf (buf + length, sl, "&quot;"); break;
    case '\'': if (quote_type != XML_ESCAPE_SINGLE_QUOTE) goto literal;
	       length += snprintf (buf + length, sl, "&apos;"); break;
    case '&':  length += snprintf (buf + length, sl, "&amp;");  break;
    case '<':  length += snprintf (buf + length, sl, "&lt;");   break;
    case '>':  length += snprintf (buf + length, sl, "&gt;");   break;
    case 127:  length += snprintf (buf + length, sl, "&#127;"); break;
    case '\t':
    case '\n':
      literal: if (buf)	buf[length] = c; ++length; break;
    default:   if (c >= ' ') goto literal;
	       length += snprintf (buf + length, sl, "&#%d;", c); break;
    }
  if (buf)
    buf[length] = 0;
  return length + 1;
}

char *exml_escape_string (const char *s, exml_escape_quote_t quote_type)
{
  char *buf = calloc (1, exml_escape_string_internal (NULL, s, quote_type));
  return buf ? (exml_escape_string_internal (buf, s, quote_type), buf) : NULL;
}

static void exml_parser_dump_node (const exml_node_t *node, int indent) {

  exml_property_t *p;
  exml_node_t     *n;
  int             l;

  printf ("%*s<%s ", indent, "", node->name);

  l = strlen (node->name);

  p = node->props;
  while (p) {
    char *value = exml_escape_string (p->value, XML_ESCAPE_SINGLE_QUOTE);
    printf ("%s='%s'", p->name, value);
    free (value);
    p = p->next;
    if (p) {
      printf ("\n%*s", indent+2+l, "");
    }
  }
  printf (">\n");

  n = node->child;
  while (n) {

    exml_parser_dump_node (n, indent+2);

    n = n->next;
  }

  printf ("%*s</%s>\n", indent, "", node->name);
}

void exml_parser_dump_tree (const exml_node_t *node) {
  exml_parser_dump_node (node, 0);
}
