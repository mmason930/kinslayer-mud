#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "handler.h"
#include "screen.h"
#include "olc.h"
#include "constants.h"

#include "UserEmailAddress.h"
#include "UserEmailAddressConfirmation.h"
#include "Descriptor.h"

#include "MailUtil.h"
#include "CharacterUtil.h"
#include "MiscUtil.h"
#include "StringUtil.h"

ACMD(do_email);

void userEmailAddressEditorDisplayMainMenu(Descriptor *descriptor)
{
	get_char_cols( descriptor->character );
	OLC *olc = descriptor->olc;
	int counter = 1;

	descriptor->send("Nr  Email Address           Confirmed   Created Date  Confirmed Date\r\n");
	descriptor->send("----------------------------------------------------------------------\r\n");
	for(auto iter = olc->userEmailAddresses.begin();iter != olc->userEmailAddresses.end();++iter)
	{
		UserEmailAddress *userEmailAddress = (*iter);
		bool isConfirmed = userEmailAddress->getConfirmed();
		std::string isConfirmedString = StringUtil::yesNo(isConfirmed);
		std::string createdDateString = DateTime::parse("%Y-%m-%d", userEmailAddress->getCreatedDatetime());
		std::string confirmedDateString = userEmailAddress->getConfirmed() ? DateTime::parse("%Y-%m-%d", userEmailAddress->getConfirmedDatetime()) : "";
        descriptor->send("%s%s%2d%s  %-23s   %s%s%-3s%s       %s    %s\r\n", bld, yel, counter, nrm, StringUtil::truncateWithEllipses(userEmailAddress->getEmailAddress(), 23).c_str(), bld, (isConfirmed ? grn : red), isConfirmedString.c_str(), nrm, createdDateString.c_str(), confirmedDateString.c_str());

		++counter;
	}
	descriptor->send("\r\n");
	descriptor->send("%sA%s) Add Email\r\n", grn, nrm);
	descriptor->send("%sC%s) Confirm Email\r\n", grn, nrm);
	descriptor->send("%sR%s) Resend Confirmation\r\n", grn, nrm);
	descriptor->send("%sQ%s) Quit\r\n", grn, nrm);
	descriptor->send("Choose an option:");

	OLC_MODE( descriptor ) = USER_EMAIL_EDITOR_MAIN;
}

void parseUserEmailAddressEditor(Descriptor *descriptor, const std::string &arg)
{
	OLC *olc = descriptor->olc;
	get_char_cols(descriptor->character);
	switch( OLC_MODE(descriptor) )
	{
	case USER_EMAIL_EDITOR_MAIN:
		switch( UPPER( arg[0] ) )
		{
		case 'A':
			descriptor->send("Enter the email address you wish to add:\r\n");
			OLC_MODE(descriptor) = USER_EMAIL_EDITOR_ADD_EMAIL;
			break;
		case 'C':
			descriptor->send("Which email address are you confirming?\r\n");
			OLC_MODE(descriptor) = USER_EMAIL_EDITOR_CONFIRM_EMAIL_SELECT;
			break;
		case 'R':
			descriptor->send("Which email address confirmation code would you like to have resent?(choose the email number from the main menu): \r\n");
			OLC_MODE(descriptor) = USER_EMAIL_EDITOR_RESEND_CONFIRMATION_SELECT;
			break;
		case 'Q':
			cleanup_olc(descriptor, CLEANUP_ALL);
			break;
		default:
			userEmailAddressEditorDisplayMainMenu(descriptor);
			descriptor->send("Invalid option. Try again:\r\n");
			break;
		}
		break;
	case USER_EMAIL_EDITOR_RESEND_CONFIRMATION_SELECT:
	{
		int emailNumber = atoi(arg.c_str()) - 1;
		if(!MiscUtil::isInt(arg) || emailNumber < 0 || emailNumber >= olc->userEmailAddresses.size())
		{
			descriptor->send("%s%sThe email you selected is invalid. You must select the email number from the list on the main menu.%s\r\n", bld, red, nrm);
			userEmailAddressEditorDisplayMainMenu(descriptor);
			break;
		}

		UserEmailAddress *userEmailAddress = NULL;
		for(auto iter = olc->userEmailAddresses.begin();emailNumber >= 0;--emailNumber, ++iter)
		{
			userEmailAddress = (*iter);
		}

		//Only resend the confirmation if the email has not been confirmed yet.
		if(userEmailAddress->getConfirmed() == true)
		{
			descriptor->send("%s%sThis email address has already been confirmed.%s\r\n", bld, red, nrm);
			userEmailAddressEditorDisplayMainMenu(descriptor);
			break;
		}

		UserEmailAddressConfirmation *userEmailAddressConfirmation = CharacterUtil::getUserEmailAddressConfirmationByUserEmailAddressId(gameDatabase, userEmailAddress->getId());

		if(userEmailAddressConfirmation == NULL)
		{//For some reason there is no confirmation code in the database for this email. Let's create one right now.
			userEmailAddressConfirmation = new UserEmailAddressConfirmation();
			userEmailAddressConfirmation->setUserEmailAddressId(userEmailAddress->getId());
			userEmailAddressConfirmation->setConfirmationKey(StringUtil::getRandomString(20));

			CharacterUtil::putUserEmailAddressConfirmation(gameDatabase, userEmailAddressConfirmation);
		}

		CharacterUtil::sendUserEmailAddressConfirmationEmail(gameDatabase, descriptor->character, userEmailAddress, userEmailAddressConfirmation);
		delete userEmailAddressConfirmation;
		
		descriptor->send("%s%sAn email has been sent to %s with a confirmation code.%s\r\n", bld, grn, userEmailAddress->getEmailAddress().c_str(), nrm);
		userEmailAddressEditorDisplayMainMenu(descriptor);
		break;
	}
	case USER_EMAIL_EDITOR_ADD_EMAIL:
	{
		//Confirm that the email address they entered is valid.
		if(!MiscUtil::isValidEmailAddress(arg))
		{
			descriptor->send("%s%sYou have entered an invalid email address%s\r\n", bld, red, nrm);
			userEmailAddressEditorDisplayMainMenu(descriptor);
			break;
		}

		//Confirm that the email does not already exist in the user's list.
		bool alreadyRegistered = false;
		for(auto iter = olc->userEmailAddresses.begin();iter != olc->userEmailAddresses.end();++iter)
		{
			UserEmailAddress *userEmailAddress = (*iter);
			if(!str_cmp(userEmailAddress->getEmailAddress(), arg))
			{
				descriptor->send("%s%sThis email address is already registered to this account.%s\r\n", bld, red, nrm);
				userEmailAddressEditorDisplayMainMenu(descriptor);
				alreadyRegistered = true;
				break;
			}
		}

		if(alreadyRegistered)
			break;

		olc->userEmailAddress = new UserEmailAddress();
		olc->userEmailAddress->setConfirmed(false);
		olc->userEmailAddress->setCreatedDatetime(DateTime());
		olc->userEmailAddress->setConfirmedDatetime(0);
		olc->userEmailAddress->setUserId(descriptor->character->getUserId());
		olc->userEmailAddress->setEmailAddress(arg);

		descriptor->send("Enter your email a second time to confirm that it is correct:\r\n");
		OLC_MODE(descriptor) = USER_EMAIL_EDITOR_ADD_EMAIL_CONFIRM;
		break;
	}
	case USER_EMAIL_EDITOR_ADD_EMAIL_CONFIRM:

		if(olc->userEmailAddress->getEmailAddress() != arg)
		{
			descriptor->send("%s%sThe emails you entered did not match. Your email has not been added.%s\r\n", bld, red, nrm);
			delete olc->userEmailAddress;
		}
		else
		{
			descriptor->send("%s%sYour email has been added to the system. You should receive an email with a confirmation number.\r\n"
							 "You can enter your confirmation number by choosing option 'C' in the main menu.%s\r\n", bld, grn, nrm);

			CharacterUtil::putUserEmailAddress(gameDatabase, olc->userEmailAddress);
			olc->userEmailAddresses.push_back(olc->userEmailAddress);

			UserEmailAddressConfirmation *userEmailAddressConfirmation = new UserEmailAddressConfirmation();
			userEmailAddressConfirmation->setUserEmailAddressId(olc->userEmailAddress->getId());
			userEmailAddressConfirmation->setConfirmationKey(StringUtil::getRandomString(20));

			CharacterUtil::putUserEmailAddressConfirmation(gameDatabase, userEmailAddressConfirmation);

			CharacterUtil::sendUserEmailAddressConfirmationEmail(gameDatabase, descriptor->character, olc->userEmailAddress, userEmailAddressConfirmation);
			delete userEmailAddressConfirmation;
		}
		olc->userEmailAddress = NULL;
		userEmailAddressEditorDisplayMainMenu(descriptor);
		break;
	case USER_EMAIL_EDITOR_CONFIRM_EMAIL_SELECT:
	{
		int emailNumber = atoi(arg.c_str()) - 1;
		if(!MiscUtil::isInt(arg) || emailNumber < 0 || emailNumber >= olc->userEmailAddresses.size())
		{
			descriptor->send("%s%sThe email you selected is invalid. You must select the email number from the list on the main menu.%s\r\n", bld, red, nrm);
			userEmailAddressEditorDisplayMainMenu(descriptor);
			break;
		}

		UserEmailAddress *userEmailAddress = NULL;
		for(auto iter = olc->userEmailAddresses.begin();emailNumber >= 0;--emailNumber, ++iter)
		{
			userEmailAddress = (*iter);
		}

		olc->userEmailAddress = userEmailAddress;
		descriptor->send("Please enter the verification code: \r\n");
		OLC_MODE(descriptor) = USER_EMAIL_EDITOR_CONFIRM_EMAIL_CODE;
		break;
	}
	case USER_EMAIL_EDITOR_CONFIRM_EMAIL_CODE:
	{
		UserEmailAddressConfirmation *userEmailAddressConfirmation = CharacterUtil::getUserEmailAddressConfirmation(gameDatabase, arg);
		UserEmailAddress *userEmailAddress = olc->userEmailAddress;//Altering this object should update the email object in the user's cache list as well.

		//If the confirmation is null, then no email confirmation exists with this verification code.
		if(userEmailAddressConfirmation == NULL || userEmailAddress->getId() != userEmailAddressConfirmation->getUserEmailAddressId())
		{
			descriptor->send("%s%sThe verification code you entered is incorrect.%s\r\n", bld, red, nrm);
		}
		else
		{
			descriptor->send("%s%sYour email addess has been confirmed!%s\r\n", bld, grn, nrm);
			userEmailAddress->setConfirmed(true);
			userEmailAddress->setConfirmedDatetime(DateTime());
			CharacterUtil::putUserEmailAddress(gameDatabase, userEmailAddress);
		}

		//Free up any memory we allocated.
		if(userEmailAddressConfirmation)
			delete userEmailAddressConfirmation;

		//Send them back to the main menu.
		userEmailAddressEditorDisplayMainMenu(descriptor);
		olc->userEmailAddress = NULL;//Do not free this. It is in the olc email cache list.
		break;
	}
	default:
		MudLog(BRF, LVL_IMPL, TRUE, "Invalid UserEmailEditor mode(player %s): %d", GET_NAME(descriptor->character), OLC_MODE( descriptor ));
		break;
	}
}

ACMD(do_email)
{
	char buf1[MAX_STRING_LENGTH];

	ch->desc->olc = new OLC();
	OLC_ZONE(ch->desc) = 0;
	STATE(ch->desc) = CON_EMAIL;

	ch->desc->olc->userEmailAddresses = CharacterUtil::getUserEmailAddresses(gameDatabase, ch->getUserId());
	userEmailAddressEditorDisplayMainMenu(ch->desc);

	Act("$n begins editing $s account settings.", TRUE, ch, 0, 0, TO_ROOM);
	SET_BITK(PLR_FLAGS(ch), Q_BIT(PLR_WRITING));
}
