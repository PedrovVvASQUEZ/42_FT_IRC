/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messages.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:12:10 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/14 18:37:29 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGES_HPP
# define MESSAGES_HPP

#include <string>
#include <vector>
#include <iostream>
#include <cctype>

class Messages
{
	private:
		std::string					_rawMessage;
		std::string					_prefix;
		std::string					_command;
		std::vector<std::string>	_params;
		bool						_isValid;

		// Méthodes privées de parsing
		void						_parseMessage();
		bool						_validateCommand() const;
		std::string					_trim(const std::string& str) const;

	public:
		// Constructeurs et destructeur
		Messages();
		Messages(const std::string& rawMessage);
		Messages(const Messages& other);
		~Messages();

		// Opérateur d'assignation
		Messages& operator=(const Messages& other);

		// Getters
		std::string					getRawMessage() const;
		std::string					getPrefix() const;
		std::string					getCommand() const;
		std::vector<std::string>	getParams() const;
		std::string					getParam(size_t index) const;
		size_t						getParamCount() const;
		bool						isValid() const;

		// Méthodes utilitaires
		bool						hasPrefix() const;
		bool						isNumericReply() const;
		std::string					toString() const;

		// Méthodes statiques pour créer des messages
		static Messages				createMessage(const std::string& command, const std::vector<std::string>& params);
		static Messages				createReply(const std::string& prefix, const std::string& numeric, 
											   const std::string& target, const std::string& message);
};

#endif