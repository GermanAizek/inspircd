/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2010 InspIRCd Development Team
 * See: http://wiki.inspircd.org/Credits
 *
 * This program is free but copyrighted software; see
 *	    the file COPYING for details.
 *
 * ---------------------------------------------------
 */

#ifndef INSPIRCD_LISTMODE_PROVIDER
#define INSPIRCD_LISTMODE_PROVIDER

/** Get the time as a string
 */
inline std::string stringtime()
{
	std::ostringstream TIME;
	TIME << ServerInstance->Time();
	return TIME.str();
}

/** The number of items a listmode's list may contain
 */
class ListLimit
{
public:
	std::string mask;
	unsigned int limit;
};

/** Max items per channel by name
 */
typedef std::list<ListLimit> limitlist;

/** The base class for list modes, should be inherited.
 */
class CoreExport ListModeBase : public ModeHandler
{
 protected:
	/** Numeric to use when outputting the list
	 */
	unsigned int listnumeric;
	/** Numeric to indicate end of list
	 */
	unsigned int endoflistnumeric;
	/** String to send for end of list
	 */
	std::string endofliststring;
	/** Automatically tidy up entries
	 */
	bool tidy;
	/** Config tag to check for max items per channel
	 */
	std::string configtag;
	/** Limits on a per-channel basis read from the tag
	 * specified in ListModeBase::configtag
	 */
	limitlist chanlimits;

 public:
	/** Storage key
	 */
	SimpleExtItem<modelist> extItem;

	/** Constructor.
	 * @param Instance The creator of this class
	 * @param modechar Mode character
	 * @param eolstr End of list string
	 * @pram lnum List numeric
	 * @param eolnum End of list numeric
	 * @param autotidy Automatically tidy list entries on add
	 * @param ctag Configuration tag to get limits from
	 */
	ListModeBase(Module* Creator, const std::string& Name, char modechar, const std::string &eolstr, unsigned int lnum, unsigned int eolnum, bool autotidy, const std::string &ctag = "banlist")
		: ModeHandler(Creator, Name, modechar, PARAM_ALWAYS, MODETYPE_CHANNEL),
		listnumeric(lnum), endoflistnumeric(eolnum), endofliststring(eolstr), tidy(autotidy),
		configtag(ctag), extItem(name + "_mode_list", Creator)
	{
		list = true;
		this->DoRehash();
		ServerInstance->Extensions.Register(&extItem);
	}

	virtual void DisplayList(User* user, Channel* channel);
	virtual const modelist* GetList(Channel* channel);

	virtual void DisplayEmptyList(User* user, Channel* channel)
	{
		user->WriteNumeric(endoflistnumeric, "%s %s :%s", user->nick.c_str(), channel->name.c_str(), endofliststring.c_str());
	}

	virtual void RemoveMode(Channel* channel, irc::modestacker* stack);
	virtual void RemoveMode(User*, irc::modestacker* stack);

	/** Perform a rehash of this mode's configuration data
	 */
	virtual void DoRehash();

	/** Handle the list mode.
	 * See mode.h
	 */
	virtual ModeAction OnModeChange(User* source, User*, Channel* channel, std::string &parameter, bool adding);

	/** Validate parameters.
	 * Overridden by implementing module.
	 * @param source Source user adding the parameter
	 * @param channel Channel the parameter is being added to
	 * @param parameter The actual parameter being added
	 * @return true if the parameter is valid
	 */
	virtual bool ValidateParam(User*, Channel*, std::string&)
	{
		return true;
	}

	/** Tell the user the list is too long.
	 * Overridden by implementing module.
	 * @param source Source user adding the parameter
	 * @param channel Channel the parameter is being added to
	 * @param parameter The actual parameter being added
	 * @return Ignored
	 */
	virtual bool TellListTooLong(User*, Channel*, std::string&)
	{
		return false;
	}

	/** Tell the user an item is already on the list.
	 * Overridden by implementing module.
	 * @param source Source user adding the parameter
	 * @param channel Channel the parameter is being added to
	 * @param parameter The actual parameter being added
	 */
	virtual void TellAlreadyOnList(User*, Channel*, std::string&)
	{
	}

	/** Tell the user that the parameter is not in the list.
	 * Overridden by implementing module.
	 * @param source Source user removing the parameter
	 * @param channel Channel the parameter is being removed from
	 * @param parameter The actual parameter being removed
	 */
	virtual void TellNotSet(User*, Channel*, std::string&)
	{
	}
};

#endif
