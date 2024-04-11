// Copyright (C) Explorer++ Project
// SPDX-License-Identifier: GPL-3.0-only
// See LICENSE in the top level directory

#include "pch.h"
#include "ShellBrowserHistoryHelper.h"
#include "HistoryService.h"
#include "IconFetcherMock.h"
#include "ShellBrowserFake.h"
#include "TabNavigationMock.h"
#include <gtest/gtest.h>

using namespace testing;

class ShellBrowserHistoryHelperTest : public Test
{
protected:
	void NavigateInNewTab(const std::wstring &path, unique_pidl_absolute *outputPidl)
	{
		ShellBrowserFake shellBrowser(&m_tabNavigation, &m_iconFetcher);
		ShellBrowserHistoryHelper::CreateAndAttachToShellBrowser(&shellBrowser, &m_historyService);
		ASSERT_HRESULT_SUCCEEDED(
			shellBrowser.NavigateToPath(path, HistoryEntryType::AddEntry, outputPidl));
	}

	TabNavigationMock m_tabNavigation;
	IconFetcherMock m_iconFetcher;
	HistoryService m_historyService;
};
TEST_F(ShellBrowserHistoryHelperTest, NavigationInDifferentTabs)
{
	const auto &history = m_historyService.GetHistoryItems();
	EXPECT_EQ(history.size(), 0U);

	MockFunction<void()> callback;
	m_historyService.AddHistoryChangedObserver(callback.AsStdFunction());
	EXPECT_CALL(callback, Call()).Times(3);

	unique_pidl_absolute pidlFake1;
	NavigateInNewTab(L"C:\\Fake1", &pidlFake1);
	ASSERT_EQ(history.size(), 1U);
	EXPECT_TRUE(ArePidlsEquivalent(history[0].Raw(), pidlFake1.get()));

	unique_pidl_absolute pidlFake2;
	NavigateInNewTab(L"C:\\Fake2", &pidlFake2);
	ASSERT_EQ(history.size(), 2U);
	EXPECT_TRUE(ArePidlsEquivalent(history[0].Raw(), pidlFake2.get()));

	unique_pidl_absolute pidlFake3;
	NavigateInNewTab(L"C:\\Fake3", &pidlFake3);
	ASSERT_EQ(history.size(), 3U);
	EXPECT_TRUE(ArePidlsEquivalent(history[0].Raw(), pidlFake3.get()));
}
