#ifndef _GIBBS_FIELD_H_
#define _GIBBS_FIELD_H_

namespace intelligent {

	class GibbsField;
	
	class GibbsNode {
	public:

		const GibbsField& field;

		GibbsNode( const GibbsField& field );
	};
	
	class GibbsField {
	public:

		GibbsField();
		
	};
	
}

#endif