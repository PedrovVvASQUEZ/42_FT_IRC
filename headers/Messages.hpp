/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messages.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:12:10 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/23 18:17:16 by pgrellie         ###   ########.fr       */
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

		void						_parseMessage();
		bool						_validateCommand() const;
		std::string					_trim(const std::string& str) const;

	public:

		Messages();
		Messages(const std::string& rawMessage);
		Messages(const Messages& other);
		~Messages();

		Messages& operator=(const Messages& other);

		std::string					getRawMessage() const;
		std::string					getPrefix() const;
		std::string					getCommand() const;
		std::vector<std::string>	getParams() const;
		std::string					getParam(size_t index) const;
		size_t						getParamCount() const;
		bool						isValid() const;
};

#endif