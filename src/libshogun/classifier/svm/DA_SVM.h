/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2007-2009 Christian Widmer
 * Copyright (C) 2007-2009 Max-Planck-Society
 */

#ifndef _DA_SVM_H___
#define _DA_SVM_H___


#include "lib/common.h"
#include "classifier/svm/SVM.h"
#include "classifier/svm/SVM_libsvm.h"
#include "classifier/svm/LibSVM.h"
#include "classifier/svm/SVM_light.h"

#include <stdio.h>


/** @brief class DomainAdaptiveSVM */
class CDA_SVM : public CSVMLight
{

	public:

		/** default constructor */
		CDA_SVM();


		/** constructor
		 *
		 * @param C cost constant C
		 * @param k kernel
		 * @param lab labels
		 * @param presvm trained SVM to regularize against
		 * @param B trade-off constant B
		 */
		CDA_SVM(float64_t C, CKernel* k, CLabels* lab, CSVM* presvm, float64_t B);


		/** destructor */
		virtual ~CDA_SVM();


		/** init SVM
		 *
		 * @param presvm trained SVM to regularize against
		 * @param B trade-off constant B
		 * */
		void init(CSVM* presvm, float64_t B);


		/** train SVM classifier
		 *
		 * @return whether training was successful
		 */
		virtual bool train();


		/** get classifier type
		 *
		 * @return classifier type LIGHT
		 */
		virtual inline EClassifierType get_classifier_type() { return CT_DASVM; }


		/** classify objects
		 *
		 * @param data (test)data to be classified
		 * @return classified labels
		 */
		virtual CLabels* classify(CFeatures* data);


		/** returns SVM that is used as prior information
		 *
		 * @return presvm
		 */
		virtual CSVM* get_presvm();


		/** getter for regularization parameter B
		 *
		 * @return regularization parameter B
		 */
		virtual float64_t get_B();


	private:

#ifdef HAVE_BOOST_SERIALIZATION
		friend class boost::serialization::access;
		// When the class Archive corresponds to an output archive, the
		// & operator is defined similar to <<.  Likewise, when the class Archive
		// is a type of input archive the & operator is defined similar to >>.
		template<class Archive>
		void serialize(Archive & ar, const unsigned int archive_version)
		{

			SG_DEBUG("archiving CDA_SVM\n");

			// serialize base class
			ar & boost::serialization::base_object<CSVMLight>(*this);

			// serialize remaining fields
			ar & presvm;

			ar & B;

			ar & train_factor;

			SG_DEBUG("done archiving CDA_SVM\n");

		}
#endif //HAVE_BOOST_SERIALIZATION

	protected:

		/** check sanity of presvm
		 *
		 * @return true if sane, throws SG_ERROR otherwise
		 */
		virtual bool is_presvm_sane();


		/** SVM to regularize against */
		CSVM* presvm;


		/** regularization parameter B */
		float64_t B;


		/** flag to switch off regularization in training */
		float64_t train_factor;

};
#endif

