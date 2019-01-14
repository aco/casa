#pragma once

typedef enum
{
	/// <summary>
	/// A node update request.
	/// </summary>
	CMDNODE = 0,
	/// <summary>
	/// An ambient/system report.
	/// </summary>
	CMDREPORT = 1,
	/// <summary>
	/// A notification to dispatch.
	/// </summary>
	CMDNOTIFICATION = 2,
	/// <summary>
	/// A confirmation of receipt.
	/// </summary>
	CMDCONFIRMATION = 3,
	/// <summary>
	/// A client profile identification
	/// </summary>
	CMDIDENTIFICATION = 4
} Command;