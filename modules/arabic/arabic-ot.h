/* This file is taken from the FreeType (1) tree. It's been reindented
 * to roughly match Pango guidelines (in anticipation of future changes),
 * but not otherwise much altered.
 */

/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2000 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  arabic -- An implementation of the contextual algorithm given in the    */
/*  Unicode 2.0 book to assign the `isolated', `initial', `medial', and     */
/*  `final' properties to an input string of character codes for the Arabic */
/*  script.                                                                 */
/*                                                                          */
/****************************************************************************/


#include <pango/pango-ot.h>


enum  joining_type_
{
  isolated = 1,       /* nominal        */
  final    = 2,       /* right_joining  */
  initial  = 4,       /* left_joining   */
  medial   = 8        /* double_joining */
};

typedef enum joining_type_  joining_type;


  /* A glyph's property value as needed by e.g. TT_GSUB_Apply_String()
     specifies which features should *not* be applied                  */

enum  arabic_glyph_property_
{
  isolated_p = final    | initial | medial,
  final_p    = isolated | initial | medial,
  initial_p  = isolated | final   | medial,
  medial_p   = isolated | final   | initial
};

typedef enum arabic_glyph_property_  arabic_glyph_property;


enum  joining_class_
{
  right,
  left,               /* not used */
  dual,
  causing,
  none,
  transparent
};

typedef enum joining_class_  joining_class;


FT_Error  Arabic_Assign_Properties (gunichar   *string,
				    gulong     *properties,
				    int         length);
#if 0
TT_Error  Build_Arabic_Glyph_Properties (TT_CharMap        char_map,
					 TT_UShort         max_glyphs,
					 TTO_GDEFHeader**  gdef );
#endif


/* End */
