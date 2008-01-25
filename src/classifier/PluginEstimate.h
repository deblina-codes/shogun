/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2008 Soeren Sonnenburg
 * Copyright (C) 1999-2008 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#ifndef _PLUGINESTIMATE_H___
#define _PLUGINESTIMATE_H___

#include "base/SGObject.h"
#include "features/StringFeatures.h"
#include "features/Labels.h"
#include "distributions/hmm/LinearHMM.h"

/** class PluginEstimate */
class CPluginEstimate: public CSGObject
{
	public:
		/** default constructor */
		CPluginEstimate();
		~CPluginEstimate();

		/** train the estimate
		 *
		 * @param features features to train
		 * @param labels features' labels
		 * @param pos_pseudo pseudo for positive model
		 * @param neg_pseudo pseudo for negative model
		 * @return if training was successful
		 */
		bool train(CStringFeatures<WORD>* features, CLabels* labels,
			DREAL pos_pseudo=1e-10, DREAL neg_pseudo=1e-10);

		/** test the estimate
		 *
		 * @return test result
		 */
		DREAL* test();

		/** set test features
		 *
		 * @param f features for test
		 */
		void set_testfeatures(CStringFeatures<WORD>* f) { test_features=f; }

		/// classify all test features
		CLabels* classify(CLabels* output=NULL);

		/// classify the test feature vector indexed by idx
		DREAL classify_example(INT idx);

		/** obsolete posterior log odds
		 *
		 * @param vector vector
		 * @param len len
		 * @return something floaty
		 */
		inline DREAL posterior_log_odds_obsolete(WORD* vector, INT len)
		{
			return pos_model->get_log_likelihood_example(vector, len) - neg_model->get_log_likelihood_example(vector, len);
		}

		/** get log odds parameter-wise
		 *
		 * @param obs observation
		 * @param position position
		 * @return log odd at position
		 */
		inline DREAL get_parameterwise_log_odds(WORD obs, INT position)
		{
			return pos_model->get_positional_log_parameter(obs, position) - neg_model->get_positional_log_parameter(obs, position);
		}

		/** get obsolete positive log derivative
		 *
		 * @param obs observation
		 * @param pos position
		 * @return positive log derivative
		 */
		inline DREAL log_derivative_pos_obsolete(WORD obs, INT pos)
		{
			return pos_model->get_log_derivative_obsolete(obs, pos);
		}

		/** get obsolete negative log derivative
		 *
		 * @param obs observation
		 * @param pos position
		 * @return negative log derivative
		 */
		inline DREAL log_derivative_neg_obsolete(WORD obs, INT pos)
		{
			return neg_model->get_log_derivative_obsolete(obs, pos);
		}

		/** get model parameters
		 *
		 * @param pos_params parameters of positive model
		 * @param neg_params parameters of negative model
		 * @param seq_length sequence length
		 * @param num_symbols numbe of symbols
		 * @return if operation was successful
		 */
		inline bool get_model_params(DREAL*& pos_params, DREAL*& neg_params, INT &seq_length, INT &num_symbols)
		{
			INT num;

			if ((!pos_model) || (!neg_model))
			{
				SG_ERROR( "no model available\n");
				return false;
			}

			pos_model->get_log_transition_probs(&pos_params, &num);
			neg_model->get_log_transition_probs(&neg_params, &num);

			seq_length = pos_model->get_sequence_length();
			num_symbols = pos_model->get_num_symbols();
			ASSERT(pos_model->get_num_model_parameters() == neg_model->get_num_model_parameters());
			ASSERT(pos_model->get_num_symbols() == neg_model->get_num_symbols());
			return true;
		}

		/** set model parameters
		 * @param pos_params parameters of positive model
		 * @param neg_params parameters of negative model
		 * @param seq_length sequence length
		 * @param num_symbols numbe of symbols
		 */
		inline void set_model_params(const DREAL* pos_params, const DREAL* neg_params, INT seq_length, INT num_symbols)
		{
			INT num_params;

			if (pos_model)
				delete pos_model;

			pos_model = new CLinearHMM(seq_length, num_symbols);

			if (neg_model)
				delete neg_model;

			neg_model = new CLinearHMM(seq_length, num_symbols);

			ASSERT(pos_model);
			ASSERT(neg_model);

			num_params=pos_model->get_num_model_parameters();
			ASSERT(seq_length*num_symbols == num_params);
			ASSERT(num_params == neg_model->get_num_model_parameters());

			pos_model->set_log_transition_probs(pos_params, num_params);
			neg_model->set_log_transition_probs(neg_params, num_params);
		}

		/** get number of parameters
		 *
		 * @return number of parameters
		 */
		inline INT get_num_params()
		{
			return pos_model->get_num_model_parameters()+neg_model->get_num_model_parameters();
		}
		
		/** check models
		 *
		 * @return if one of the two models is invalid
		 */
		inline bool check_models()
		{
			return ( (pos_model!=NULL) && (neg_model!=NULL) );
		}

	protected:
		/** positive model */
		CLinearHMM* pos_model;
		/** negative model */
		CLinearHMM* neg_model;
		/** test features */
		CStringFeatures<WORD>* test_features;
};
#endif
